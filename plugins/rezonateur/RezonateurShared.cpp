#include "RezonateurShared.hpp"

void InitParameter(uint32_t index, Parameter &parameter)
{
    switch (index) {
    case pIdBypass:
        parameter.designation = kParameterDesignationBypass;
        break;
    default:
        DISTRHO_SAFE_ASSERT(false);
    }
}
