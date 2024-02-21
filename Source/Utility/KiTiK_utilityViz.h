/*
  ==============================================================================

    KiTiK_utilityViz.h
    Created: 4 Jan 2024 4:26:39pm
    Author:  kylew

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

    struct FFTData
    {
        FFTData();
        ~FFTData();
        void pushNextSampleIntoFifo(juce::AudioBuffer<float> buffer);
        friend class FFTComp;

    protected:

        enum
        {
            fftOrder = 12,
            fftSize = 1 << fftOrder,
            scopeSize = fftSize
        };

        juce::dsp::FFT forwardFFT{ fftOrder };
        juce::dsp::WindowingFunction<float> window{ fftSize, juce::dsp::WindowingFunction<float>::hann };
        float fftData[2 * fftSize];
        float scopeData[scopeSize];
        float fifo[fftSize];
        int fifoIndex = 0;
        bool nextFFTBlockReady = false;
    };

    struct FFTComp : public juce::Component
    {
        FFTComp(FFTData& d);
        ~FFTComp();

        void paint(juce::Graphics& g) override;
        void resized() override;
        void drawNextFrame(FFTData& data);

    private:
        FFTData& data;
    };

    struct OscilloscopeData
    {
        OscilloscopeData();
        ~OscilloscopeData();

        void setBuffer(juce::AudioBuffer<float> buffer);
        friend class OscilloscopeComp;

    protected:
        juce::AudioBuffer<float> oscBuffer;

    };

    struct OscilloscopeComp : public juce::Component
    {
        OscilloscopeComp(OscilloscopeData& d);
        ~OscilloscopeComp();

        void paint(juce::Graphics&) override;
        void resized() override;

    private:

        OscilloscopeData& data;

        std::vector<float> audioPoints;
    };


