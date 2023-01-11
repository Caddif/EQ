#pragma once

enum Slope
{
    Slope12,
    Slope24,
    Slope36,
    Slope48
};

enum class FilterToChoose
{
    PeakFilter,
    LowShelfFilter,
    HighShelfFilter,
    NotchFilter
};

/**
Remember that this sruct cannot be modified. 
The only way to obtain values that are inside is by calling getChainSettings();
*/
struct EqChain {
    float HiPassFreq{ 0 }, lowPassFreq{ 0 };
    float filterFreq{ 0 }, gainInDb{ 0 }, filterQuality{ 1.f };
    float filterFreq2{ 0 }, gainInDb2{ 0 }, filterQuality2{ 1.f };
    float filterFreq3{ 0 }, gainInDb3{ 0 }, filterQuality3{ 1.f };
    Slope HiPassSlope{ Slope::Slope12 }, lowPassSlope{ Slope::Slope12 };
    FilterToChoose Filter1choice{ FilterToChoose::PeakFilter }, Filter2choice{ FilterToChoose::PeakFilter }, Filter3choice{ FilterToChoose::PeakFilter };
    float totalGain{ 0 };
    bool hfpBypass{ true }, peak1Bypass{ false }, peak2Bypass{ false }, peak3Bypass{ false }, lpfBypass{ true };
};

struct NoiseGateChain 
{
    float threshold{ -48 };
    float ratio{ 1 };
    float attack{ 0 };
    float release{ 0 };
};

struct CompressorChain
{
    float threshold{ -48 };
    float ratio{ 1 };
    float attack{ 0 };
    float release{ 0 };
};

enum Chainpositons
{
    NoiseGate,
    Compressor,
    HiPass,
    Filter1,
    Filter2,
    Filter3,
    LowPass,
    Gain
};

template<int Id, typename Chaintype, typename CoefficientType>
void update(Chaintype& chain, const CoefficientType& coefficeints)
{
    updateCoeffs(chain.template get<Id>().coefficients, coefficeints[Id]);
    chain.template setBypassed<Id>(false);
}

template<typename ChainType, typename CoefficientType>
void updatePassFilter(ChainType& chain, const CoefficientType& cutCoeffs, const Slope& passSlope)
{
    chain.template setBypassed<0>(true);
    chain.template setBypassed<1>(true);
    chain.template setBypassed<2>(true);
    chain.template setBypassed<3>(true);

    switch (passSlope)
    {
        case Slope48:
        {
            update<3>(chain, cutCoeffs);
        }
        case Slope36:
        {
            update<2>(chain, cutCoeffs);
        }
        case Slope24:
        {
            update<1>(chain, cutCoeffs);
        }
        case Slope12:
        {
            update<0>(chain, cutCoeffs);
        }
    }

}