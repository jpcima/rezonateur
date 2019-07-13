/*
  ==============================================================================

    VAStateVariableFilter.h
    Created: 10 May 2015 2:41:43pm
    Author:  Jordan Harris

    Notes:
    From The Art of VA Filter Design, by Vadim Zavalishin

    TPT State Variable Filter:

    TPT -> Topology Preserving Transform

    This filter is based off of the trapezoidal integrator, which produces
    a bilinear transformation. This filter does the proper cutoff prewarping
    needed for the TPT structure, as explained by Zavalishin. It also handles
    the problem of delayless feedback loops that the trapezoidal integrators
    introduce.

    The SVF has two feedback paths sharing a common path segment. In order to
    obtain a single feedback equation (rather than equation system), a signal
    on the common path should be selected as the unknown variable. The HP filter
    path is chosen as the zero-delay feedback equation's unknown in this filter.
    Using the HP filter path, the rest of the filter's signals in the structure
    can be defined.

  ==============================================================================
*/

#ifndef VASTATEVARIABLEFILTER_H_INCLUDED
#define VASTATEVARIABLEFILTER_H_INCLUDED

#include <complex>

//==============================================================================

/** The type of filter that the State Variable Filter will output. */
enum SVFType {
    SVFLowpass = 0,
    SVFBandpass,
    SVFHighpass,
    SVFUnitGainBandpass,
    SVFBandShelving,
    SVFNotch,
    SVFAllpass,
    SVFPeak
};

//==============================================================================
class VAStateVariableFilter {
public:
    /** Create and initialize the filter with default values defined in constructor. */
    VAStateVariableFilter();

    //------------------------------------------------------------------------------

    ~VAStateVariableFilter();

    //------------------------------------------------------------------------------

    /**    Sets the type of the filter that processAudioSample() or processAudioBlock() will
        output. This filter can choose between 8 different types using the enums listed
        below or the int given to each.
        0: SVFLowpass
        1: SVFBandpass
        2: SVFHighpass
        3: SVFUnitGainBandpass
        4: SVFBandShelving
        5: SVFNotch
        6: SVFAllpass
        7: SVFPeak
    */
    void setFilterType(int newType);

    //------------------------------------------------------------------------------
    /**    Used for changing the filter's cutoff parameter linearly by frequency (Hz) */
    void setCutoffFreq(double newCutoffFreq);

    //------------------------------------------------------------------------------
    /** Used for setting the resonance amount. This is then converted to a Q
        value, which is used by the filter.
        Range: (0-1)
    */
    void setResonance(double newResonance);

    //------------------------------------------------------------------------------
    /** Used for setting the filter's Q amount. This is then converted to a
        damping parameter called R, which is used in the original filter.
    */
    void setQ(double newQ);

    //------------------------------------------------------------------------------
    /**    Sets the gain of the shelf for the BandShelving filter only. */
    void setShelfGain(double newGain);

    //------------------------------------------------------------------------------
    /**    Statically set the filters parameters. */
    void setFilter(int newType, double newCutoff,
                   double newResonance, double newShelfGain);

    //------------------------------------------------------------------------------
    /**    Set the sample rate used by the host. Needs to be used to accurately
        calculate the coefficients of the filter from the cutoff.
        Note: This is often used in AudioProcessor::prepareToPlay
    */
    void setSampleRate(double newSampleRate);

    //------------------------------------------------------------------------------
    /**    Performs the actual processing.
    */
    void process(float gain, const float *input, float *output, unsigned count);

    //------------------------------------------------------------------------------
    /**    Reset the state variables.
    */
    void clear();

    //------------------------------------------------------------------------------
    /**    Compute the transfer function at given frequency.
    */
    std::complex<double> calcTransfer(double freq) const;

    //------------------------------------------------------------------------------


    double getCutoffFreq() const { return cutoffFreq; }

    double getFilterType() const { return filterType; }

    double getQ() const { return Q; }

    double getShelfGain() const { return shelfGain; }

private:
    //==============================================================================
    //    Calculate the coefficients for the filter based on parameters.
    void calcFilter();

    //
    template <int FilterType>
    void processInternally(float gain, const float *input, float *output, unsigned count);

    //    Parameters:
    int filterType;
    double cutoffFreq;
    double Q;
    double shelfGain;

    double sampleRate;

    //    Coefficients:
    double gCoeff;        // gain element
    double RCoeff;        // feedback damping element
    double KCoeff;        // shelf gain element

    double z1_A, z2_A;        // state variables (z^-1)

private:
    static std::complex<double> calcTransferLowpass(double w, double wc, double r);
    static std::complex<double> calcTransferBandpass(double w, double wc, double r);
    static std::complex<double> calcTransferHighpass(double w, double wc, double r);
    static std::complex<double> calcTransferUnitGainBandpass(double w, double wc, double r);
    static std::complex<double> calcTransferBandShelving(double w, double wc, double r, double k);
    static std::complex<double> calcTransferNotch(double w, double wc, double r);
    static std::complex<double> calcTransferAllpass(double w, double wc, double r);
    static std::complex<double> calcTransferPeak(double w, double wc, double r);
};

//==============================================================================
#endif  // VASTATEVARIABLEFILTER_H_INCLUDED
