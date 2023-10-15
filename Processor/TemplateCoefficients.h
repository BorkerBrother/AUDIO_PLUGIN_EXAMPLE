//
// Created by Kai on 15.10.23.
//
#include "Setting.h"

#ifndef AUDIO_PLUGIN_EXAMPLE_TEMPLATECOEFFICIENTS_H
#define AUDIO_PLUGIN_EXAMPLE_TEMPLATECOEFFICIENTS_H

// TEMPLATE UPDATE COEFFICIENTS
template<int Index, typename ChainType, typename CoefficientType>
void update(ChainType& chain, const CoefficientType& coefficients)
{
    updateCoefficients(chain.template get<Index>().coefficients, coefficients[Index]);
    chain.template setBypassed<Index>(false);
}

// TEMPLATE UPDATE CUTFILTER
template<typename ChainType, typename CoefficientType>
void updateCutFilter (ChainType& chainType,
                      const CoefficientType& coefficients,
                      const Slope& slope)
{


    chainType.template setBypassed<0>(true);
    chainType.template setBypassed<1>(true);
    chainType.template setBypassed<2>(true);
    chainType.template setBypassed<3>(true);

    switch(slope){

        case Slope_48:
        {
            update<3>(chainType, coefficients);
        }
        case Slope_36:
        {
            update<2>(chainType, coefficients);
        }
        case Slope_24:
        {
            update<1>(chainType, coefficients);
        }
        case Slope_12:
        {
            update<0>(chainType, coefficients);
        }
    }
}


#endif //AUDIO_PLUGIN_EXAMPLE_TEMPLATECOEFFICIENTS_H
