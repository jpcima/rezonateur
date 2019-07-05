#include "filter_editor.h"
#include "ui_filter_editor.h"
#include "VAStateVariableFilter.h"
#include <cmath>

FilterEditor::FilterEditor(VAStateVariableFilter &filter, bool &enable_filter, bool &invert_filter, std::mutex &mutex, QWidget *parent)
    : QWidget(parent), filter_(filter), enable_filter_(enable_filter), invert_filter_(invert_filter), mutex_(mutex), ui_(new Ui::FilterEditor)
{
    Ui::FilterEditor &ui = *ui_;
    ui.setupUi(this);

    ui.selFilterType->addItem("Low pass", SVFLowpass);
    ui.selFilterType->addItem("Band pass", SVFBandpass);
    ui.selFilterType->addItem("High pass", SVFHighpass);
    ui.selFilterType->addItem("Unit gain band pass", SVFUnitGainBandpass);
    ui.selFilterType->addItem("Band shelving", SVFBandShelving);
    ui.selFilterType->addItem("Notch", SVFNotch);
    ui.selFilterType->addItem("All pass", SVFAllpass);
    ui.selFilterType->addItem("Peak", SVFPeak);
    ui.valCutoff->setRange(0, 10000);
    ui.valQ->setRange(1, 100);
    ui.valShelfGain->setRange(0, 20);

    ui.selFilterType->setCurrentIndex(ui.selFilterType->findData(filter.getFilterType()));
    ui.valCutoff->setValue(filter.getCutoffFreq());
    ui.valQ->setValue(filter.getQ());
    ui.valShelfGain->setValue(20 * std::log10(filter.getShelfGain()));
    ui.chkEnable->setChecked(enable_filter);
    ui.chkInvert->setChecked(invert_filter);
    updateValueLabels();

    connect(ui.valCutoff, &QDial::valueChanged,
            this, [this](int value) {
                      std::unique_lock<std::mutex> lock(mutex_);
                      filter_.setCutoffFreq(value);
                      updateValueLabels();
                  });
    connect(ui.valQ, &QDial::valueChanged,
            this, [this](int value) {
                      std::unique_lock<std::mutex> lock(mutex_);
                      filter_.setQ(value);
                      updateValueLabels();
                  });
    connect(ui.valShelfGain, &QDial::valueChanged,
            this, [this](int value) {
                      std::unique_lock<std::mutex> lock(mutex_);
                      filter_.setShelfGain(std::pow(10.0, 0.05 * value));
                      updateValueLabels();
                  });
    connect(ui.selFilterType, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int index) {
                      std::unique_lock<std::mutex> lock(mutex_);
                      int type = ui_->selFilterType->itemData(index).toInt();
                      filter_.setFilterType(type);
                  });
    connect(ui.chkEnable, &QCheckBox::toggled,
            this, [this](bool checked) {
                      std::unique_lock<std::mutex> lock(mutex_);
                      enable_filter_ = checked;
                  });
    connect(ui.chkInvert, &QCheckBox::toggled,
            this, [this](bool checked) {
                      std::unique_lock<std::mutex> lock(mutex_);
                      invert_filter_ = checked;
                  });
}

FilterEditor::~FilterEditor()
{
}

void FilterEditor::updateValueLabels()
{
    Ui::FilterEditor &ui = *ui_;
    VAStateVariableFilter &filter = filter_;

    ui.lblCutoff->setText(QString::number(filter.getCutoffFreq()));
    ui.lblQ->setText(QString::number(filter.getQ()));
    ui.lblShelfGain->setText(QString::number(20 * std::log10(filter.getShelfGain())));
}
