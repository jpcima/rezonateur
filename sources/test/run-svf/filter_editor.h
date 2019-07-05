#pragma once
#include <QWidget>
#include <memory>
#include <mutex>
namespace Ui { class FilterEditor; }
class VAStateVariableFilter;

class FilterEditor : public QWidget {
public:
    FilterEditor(VAStateVariableFilter &filter, bool &enable_filter, bool &invert_filter, std::mutex &mutex, QWidget *parent = nullptr);
    ~FilterEditor();

private:
    void updateValueLabels();

private:
    VAStateVariableFilter &filter_;
    bool &enable_filter_;
    bool &invert_filter_;
    std::mutex &mutex_;
    std::unique_ptr<Ui::FilterEditor> ui_;
};
