/*
  ==============================================================================

    Params.h
    Created: 26 Jan 2024 6:20:05am
    Author:  kylew

  ==============================================================================
*/

#pragma once
#include "JuceHeader.h"

namespace Params
{
    enum names
    {
        Master_In_Gain,
        Master_Mix,
        Master_Out_Gain,

        //Crossover parameters
        Low_Mid_Crossover_Freq,
        Mid_High_Crossover_Freq,

        Select_Toolbar_One,
        Select_Toolbar_Two,
        Select_Toolbar_Three,

        Oversample_Rate,

        Saturator_One_Toggle,
        Saturator_Two_Toggle,
        Saturator_Three_Toggle,

        Clipper_One_Toggle,
        Clipper_Two_Toggle,
        Clipper_Three_Toggle,

        Waveshaper_One_Toggle,
        Waveshaper_Two_Toggle,
        Waveshaper_Three_Toggle,

        Bitcrusher_One_Toggle,
        Bitcrusher_Two_Toggle,
        Bitcrusher_Three_Toggle,

        Wavefolder_One_Toggle,
        Wavefolder_Two_Toggle,
        Wavefolder_Three_Toggle,

        //Saturator Params
        Saturator_One_Drive,
        Saturator_Two_Drive,
        Saturator_Three_Drive,

        Saturator_One_In,
        Saturator_Two_In,
        Saturator_Three_In,

        Saturator_One_Mix,
        Saturator_Two_Mix,
        Saturator_Three_Mix,

        Saturator_One_Out,
        Saturator_Two_Out,
        Saturator_Three_Out,

        //Clipper Params
        Clipper_One_Type,
        Clipper_Two_Type,
        Clipper_Three_Type,

        Clipper_One_Threshold,
        Clipper_Two_Threshold,
        Clipper_Three_Threshold,

        Clipper_One_In,
        Clipper_Two_In,
        Clipper_Three_In,

        Clipper_One_Mix,
        Clipper_Two_Mix,
        Clipper_Three_Mix,

        Clipper_One_Out,
        Clipper_Two_Out,
        Clipper_Three_Out,

        //WaveShaper Params
        Waveshaper_One_Type,
        Waveshaper_Two_Type,
        Waveshaper_Three_Type,

        Waveshaper_One_Drive_Sin,
        Waveshaper_One_Drive_Quad,
        Waveshaper_One_Drive_Factor,
        Waveshaper_One_Drive_GB,
        Waveshaper_Two_Drive_Sin,
        Waveshaper_Two_Drive_Quad,
        Waveshaper_Two_Drive_Factor,
        Waveshaper_Two_Drive_GB,
        Waveshaper_Three_Drive_Sin,
        Waveshaper_Three_Drive_Quad,
        Waveshaper_Three_Drive_Factor,
        Waveshaper_Three_Drive_GB,

        Waveshaper_One_In,
        Waveshaper_Two_In,
        Waveshaper_Three_In,

        Waveshaper_One_Mix,
        Waveshaper_Two_Mix,
        Waveshaper_Three_Mix,

        Waveshaper_One_Out,
        Waveshaper_Two_Out,
        Waveshaper_Three_Out,

        //Bitcrusher Params
        Bitcrusher_One_Rate,
        Bitcrusher_Two_Rate,
        Bitcrusher_Three_Rate,

        Bitcrusher_One_Depth,
        Bitcrusher_Two_Depth,
        Bitcrusher_Three_Depth,

        Bitcrusher_One_In,
        Bitcrusher_Two_In,
        Bitcrusher_Three_In,

        Bitcrusher_One_Mix,
        Bitcrusher_Two_Mix,
        Bitcrusher_Three_Mix,

        Bitcrusher_One_Out,
        Bitcrusher_Two_Out,
        Bitcrusher_Three_Out,

        //wavefolder params
        Wavefolder_One_Type,
        Wavefolder_Two_Type,
        Wavefolder_Three_Type,

        Wavefolder_One_Drive_Sin,
        WaveFolder_One_Drive_Tri,
        Wavefolder_Two_Drive_Sin,
        WaveFolder_Two_Drive_Tri,
        Wavefolder_Three_Drive_Sin,
        WaveFolder_Three_Drive_Tri,

        Wavefolder_One_In,
        Wavefolder_Two_In,
        Wavefolder_Three_In,

        Wavefolder_One_Mix,
        Wavefolder_Two_Mix,
        Wavefolder_Three_Mix,

        Wavefolder_One_Out,
        Wavefolder_Two_Out,
        Wavefolder_Three_Out,

        //Global Params
        Global_In,
        Global_Out,
        Global_Bypass,
        Global_mix
    };

    inline const std::map<names, juce::String>& getParams()
    {
        static std::map <names, juce::String> params = 
        {
            {Master_In_Gain, "Master In Gain"},
            {Master_Mix, "Master Mix"},
            {Master_Out_Gain, "Master Out Gain"},

            {Low_Mid_Crossover_Freq, "Low-Mid Crossover Frequency"},
            {Mid_High_Crossover_Freq, "Mid-High Crossover Frequency"},

            {Select_Toolbar_One, "Toolbar One"},
            {Select_Toolbar_Two, "Toolbar Two"},
            {Select_Toolbar_Three, "Toolbar Three"},

            {Oversample_Rate, "Oversample Rate"},

            {Saturator_One_Toggle, "Saturation One Toggle" },
            {Saturator_Two_Toggle,"Saturation Two Toggle" },
            {Saturator_Three_Toggle,"Saturation Three Toggle" },

            {Clipper_One_Toggle, "Clipper One Toggle" },
            {Clipper_Two_Toggle, "Clipper Two Toggle"},
            {Clipper_Three_Toggle, "Clipper Three Toggle"},

            {Waveshaper_One_Toggle, "Waveshaper One Toggle"},
            {Waveshaper_Two_Toggle, "Waveshaper Two Toggle"},
            {Waveshaper_Three_Toggle, "Waveshaper Three Toggle" },

            {Bitcrusher_One_Toggle, "Bitcrusher One Toggle"},
            {Bitcrusher_Two_Toggle, "Bitcrusher Two Toggle"},
            {Bitcrusher_Three_Toggle, "Bitcrusher Three Toggle"},

            {Wavefolder_One_Toggle, "Wavefolder One Toggle"},
            {Wavefolder_Two_Toggle, "Wavefolder Two Toggle"},
            {Wavefolder_Three_Toggle, "Wavefolder Three Toggle"},

            {Saturator_One_Drive, "Saturator One Drive"},
            {Saturator_Two_Drive, "Saturator Two Drive"},
            {Saturator_Three_Drive, "Saturator Three Drive"},

            {Saturator_One_In, "Saturator One In Gain"},
            {Saturator_Two_In, "Saturator Two In Gain"},
            {Saturator_Three_In, "Saturator Three In Gain"},

            {Saturator_One_Mix, "Saturator One Mix"},
            {Saturator_Two_Mix, "Saturator Two Mix"},
            {Saturator_Three_Mix, "Saturator Three Mix"},

            {Saturator_One_Out, "Saturator One Out Gain"},
            {Saturator_Two_Out, "Saturator Two Out Gain"},
            {Saturator_Three_Out, "Saturator Three Out Gain"},

            {Clipper_One_Type, "Clipper One Type"},
            {Clipper_Two_Type, "Clipper Two Type"},
            {Clipper_Three_Type, "Clipper Three Type"},

            {Clipper_One_Threshold, "Clipper One Threshold"},
            {Clipper_Two_Threshold, "Clipper Two Threshold"},
            {Clipper_Three_Threshold, "Clipper Three Threshold"},

            {Clipper_One_In, "Clipper One In Gain"},
            {Clipper_Two_In, "Clipper Two In Gain"},
            {Clipper_Three_In, "Clipper Three In Gain"},

            {Clipper_One_Mix, "Clipper One Mix"},
            {Clipper_Two_Mix, "Clipper Two Mix"},
            {Clipper_Three_Mix, "Clipper Three Mix"},

            {Clipper_One_Out, "Clipper One Out Gain"},
            {Clipper_Two_Out, "Clipper Two Out Gain"},
            {Clipper_Three_Out, "Clipper Three Out Gain"},

            {Waveshaper_One_Type, "Waveshaper One Type"},
            {Waveshaper_Two_Type, "Waveshaper Two Type"},
            {Waveshaper_Three_Type, "Waveshaper Three Type"},

            {Waveshaper_One_Drive_Sin, "Sin One Drive"},
            {Waveshaper_One_Drive_Quad, "Quad One Drive"},
            {Waveshaper_One_Drive_Factor, "Factor One Drive"},
            {Waveshaper_One_Drive_GB, "GB One Drive"},
            {Waveshaper_Two_Drive_Sin, "Sin Two Drive"},
            {Waveshaper_Two_Drive_Quad, "Quad Two Drive"},
            {Waveshaper_Two_Drive_Factor, "Factor Two Drive"},
            {Waveshaper_Two_Drive_GB, "GB Two Drive"},
            {Waveshaper_Three_Drive_Sin, "Sin Three Drive"},
            {Waveshaper_Three_Drive_Quad, "Quad Three Drive"},
            {Waveshaper_Three_Drive_Factor, "Factor Three Drive"},
            {Waveshaper_Three_Drive_GB, "GB Three Drive"},

            {Waveshaper_One_In, "Waveshaper One In Gain"},
            {Waveshaper_Two_In, "Waveshaper Two In Gain"},
            {Waveshaper_Three_In, "Waveshaper Three In Gain"},

            {Waveshaper_One_Mix, "Waveshaper One Mix"},
            {Waveshaper_Two_Mix, "Waveshaper Two Mix"},
            {Waveshaper_Three_Mix, "Waveshaper Three Mix"},

            {Waveshaper_One_Out, "Waveshaper One Out Gain"},
            {Waveshaper_Two_Out, "Waveshaper Two Out Gain"},
            {Waveshaper_Three_Out, "Waveshaper Three Out Gain"},

            {Bitcrusher_One_Rate, "Bitcrusher One Rate"},
            {Bitcrusher_Two_Rate, "Bitcrusher Two Rate"},
            {Bitcrusher_Three_Rate, "Bitcrusher Three Rate"},

            {Bitcrusher_One_Depth, "Bitcrusher One Depth"},
            {Bitcrusher_Two_Depth, "Bitcrusher Two Depth"},
            {Bitcrusher_Three_Depth, "Bitcrusher Three Depth"},

            {Bitcrusher_One_In, "Bitcrusher One In Gain"},
            {Bitcrusher_Two_In, "Bitcrusher Two In Gain"},
            {Bitcrusher_Three_In, "Bitcrusher Three In Gain"},

            {Bitcrusher_One_Mix, "Bitcrusher One Mix"},
            {Bitcrusher_Two_Mix, "Bitcrusher Two Mix"},
            {Bitcrusher_Three_Mix, "Bitcrusher Three Mix"},

            { Bitcrusher_One_Out, "Bitcrusher One Out Gain" },
            { Bitcrusher_Two_Out, "Bitcrusher Two Out Gain" },
            { Bitcrusher_Three_Out, "Bitcrusher Three Out Gain" },

            { Wavefolder_One_Type, "Wavefolder One Type" },
            { Wavefolder_Two_Type, "Wavefolder Two Type" },
            { Wavefolder_Three_Type, "Wavefolder Three Type" },

            { Wavefolder_One_Drive_Sin, "Wavefolder One Drive Sin" },
            { WaveFolder_One_Drive_Tri, "Wavefolder One Drive Tri" },
            { Wavefolder_Two_Drive_Sin, "Wavefolder Two Drive Sin" },
            { WaveFolder_Two_Drive_Tri, "Wavefolder Two Drive Tri" },
            { Wavefolder_Three_Drive_Sin, "Wavefolder Three Drive Sin" },
            { WaveFolder_Three_Drive_Tri, "Wavefolder Three Drive Tri" },

            { Wavefolder_One_In, "Wavefolder One In" },
            { Wavefolder_Two_In, "Wavefolder Two In" },
            { Wavefolder_Three_In, "Wavefolder Three In" },

            { Wavefolder_One_Mix, "Wavefolder One Mix" },
            { Wavefolder_Two_Mix, "Wavefolder Two Mix" },
            { Wavefolder_Three_Mix, "Wavefolder Three Mix" },

            { Wavefolder_One_Out, "Waveolder One Out" },
            { Wavefolder_Two_Out, "Wavefolder Two Out" },
            { Wavefolder_Three_Out, "Wavefoldr Three Out" },

            { Global_In, "Global In Gain" },
            { Global_Out, "Global Out Gain" },
            { Global_Bypass, "Global Bypass" },
            { Global_mix, "Global Mix" }
        };

        return params;
    }
}