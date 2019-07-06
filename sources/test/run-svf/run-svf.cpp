#include "filter_editor.h"
#include "VAStateVariableFilter.h"
#include <jack/jack.h>
#include <QApplication>
#include <QMainWindow>
#include <QMessageBox>
#include <QGroupBox>
#include <QComboBox>
#include <QGridLayout>
#include <QVBoxLayout>
#include <mutex>
#include <cmath>
#include <cassert>

static constexpr unsigned FilterCount = 6;
static constexpr jack_nframes_t MaxNumFrames = 256;

enum ProcessMode {
    Process_Sum,
    Process_Product,
};

struct AudioContext {
    std::mutex mutex;
    ProcessMode mode = {};
    VAStateVariableFilter filter[FilterCount];
    bool enable_filter[FilterCount] = {};
    bool invert_filter[FilterCount] = {};
    jack_client_t *client = nullptr;
    jack_port_t *p_in = nullptr;
    jack_port_t *p_out = nullptr;
    float work_buffer[MaxNumFrames];
};

static int process(jack_nframes_t nframes, void *userdata)
{
    AudioContext *ctx = (AudioContext *)userdata;

    const float *in = (float *)jack_port_get_buffer(ctx->p_in, nframes);
    float *out = (float *)jack_port_get_buffer(ctx->p_out, nframes);

    std::unique_lock<std::mutex> lock(ctx->mutex, std::try_to_lock);
    if (!lock.owns_lock()) {
        for (jack_nframes_t i = 0; i < nframes; ++i)
            out[i] = 0;
        return 0;
    }

    switch (ctx->mode) {
    default:
        assert(false);
        /* fall through */
    case Process_Sum: {
        while (nframes > 0) {
            jack_nframes_t current = (nframes < MaxNumFrames) ? nframes : MaxNumFrames;

            for (jack_nframes_t i = 0; i < nframes; ++i)
                out[i] = 0;

            for (unsigned f = 0; f < FilterCount; ++f) {
                if (!ctx->enable_filter[f])
                    continue;
                VAStateVariableFilter &filter = ctx->filter[f];
                float *temp = ctx->work_buffer;

                filter.process(in, temp, nframes);

                if (ctx->invert_filter[f]) {
                    for (jack_nframes_t i = 0; i < nframes; ++i)
                        temp[i] = -temp[i];
                }

                for (jack_nframes_t i = 0; i < nframes; ++i)
                    out[i] += temp[i];
            }

            nframes -= current;
            in += current;
            out += current;
        }
        break;
    }
    case Process_Product:
        for (jack_nframes_t i = 0; i < nframes; ++i)
            out[i] = in[i];

        for (unsigned f = 0; f < FilterCount; ++f) {
            if (!ctx->enable_filter[f])
                continue;
            VAStateVariableFilter &filter = ctx->filter[f];
            filter.process(out, out, nframes);
        }
        break;
    }

    return 0;
}

///
class Window : public QMainWindow {
public:
    explicit Window(AudioContext *ctx)
        : QMainWindow(), ctx_(ctx)
    {
        QWidget *widget = new QWidget;
        setCentralWidget(widget);

        QVBoxLayout *top = new QVBoxLayout;
        widget->setLayout(top);

        {
            QGroupBox *grp = new QGroupBox;
            top->addWidget(grp);
            grp->setTitle("Mode");
            QVBoxLayout *lgrp = new QVBoxLayout;
            grp->setLayout(lgrp);
            QComboBox *cb = new QComboBox;
            lgrp->addWidget(cb);
            cb->addItem("Additive", Process_Sum);
            cb->addItem("Multiplicative", Process_Product);

            connect(cb, QOverload<int>::of(&QComboBox::currentIndexChanged),
                    this, [this, cb](int index) {
                              std::unique_lock<std::mutex> lock(ctx_->mutex);
                              ctx_->mode = (ProcessMode)cb->itemData(index).toInt();
                          });
        }

        QGridLayout *grid = new QGridLayout;
        top->addLayout(grid);
        grid->setContentsMargins(0, 0, 0, 0);

        for (unsigned f = 0; f < FilterCount; ++f) {
            QGroupBox *grp = new QGroupBox;
            grid->addWidget(grp, f % 3, f / 3);
            grp->setTitle(QString("Filter %0").arg(f + 1));

            QVBoxLayout *lgrp = new QVBoxLayout;
            grp->setLayout(lgrp);

            lgrp->addWidget(new FilterEditor(ctx->filter[f], ctx->enable_filter[f], ctx->invert_filter[f], ctx->mutex));
        }
    }

    AudioContext *ctx_ = nullptr;
};

///
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("State Variable Filter");

    AudioContext ctx;
    ctx.client = jack_client_open("State Variable Filter", JackNoStartServer, nullptr);
    if (!ctx.client) {
        QMessageBox::critical(nullptr, "Error", "Cannot open JACK client.");
        return 1;
    }

    if (!(ctx.p_in = jack_port_register(ctx.client, "in", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0)) ||
        !(ctx.p_out = jack_port_register(ctx.client, "out", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0))) {
        QMessageBox::critical(nullptr, "Error", "Cannot register JACK ports.");
        return 1;
    }

    double samplerate = jack_get_sample_rate(ctx.client);
    for (unsigned f = 0; f < FilterCount; ++f)
        ctx.filter[f].setSampleRate(samplerate);

    jack_set_process_callback(ctx.client, &process, &ctx);

    Window win(&ctx);
    win.setWindowTitle(app.applicationDisplayName());
    win.adjustSize();
    win.setFixedSize(win.size());

    if (jack_activate(ctx.client) != 0) {
        QMessageBox::critical(nullptr, "Error", "Cannot activate JACK client.");
        return 1;
    }

    win.show();

    return app.exec();
}
