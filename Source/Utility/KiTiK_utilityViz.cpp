/*
  ==============================================================================

    KiTiK_utilityViz.cpp
    Created: 4 Jan 2024 4:26:39pm
    Author:  kylew

  ==============================================================================
*/

#include "KiTiK_utilityViz.h"
#include "../PluginProcessor.h"


    FFTData::FFTData()
    {
    }

    FFTData::~FFTData()
    {
    }

    //=======================================FFTData===================================
    void FFTData::pushNextSampleIntoFifo(juce::AudioBuffer<float> buffer)
    {
        auto* data = buffer.getReadPointer(0);

        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            if (fifoIndex == fftSize)
            {
                if (!nextFFTBlockReady)
                {
                    juce::zeromem(fftData, sizeof(fftData));
                    memcpy(fftData, fifo, sizeof(fifo));
                    nextFFTBlockReady = true;
                }

                fifoIndex = 0;
            }

            fifo[fifoIndex++] = data[i];
        }
    }
    //=======================================FFTData===================================

    //=======================================FFT=======================================
    FFTComp::FFTComp(FFTData& d)
        : data(d) {}

    FFTComp::~FFTComp() {}

    void FFTComp::paint(juce::Graphics& g)
    {
        auto bounds = getLocalBounds();
        float width = bounds.getWidth();
        float height = bounds.getHeight();

        if (data.nextFFTBlockReady)
        {
            drawNextFrame(data);
            data.nextFFTBlockReady = false;
        }

        juce::Path p;

        p.startNewSubPath(0, height);

        for (int i = 1; i < data.scopeSize; ++i)
        {
            auto point = juce::jmap(data.scopeData[i], 0.f, 1.f, (height - 1), 0.f);

            p.lineTo(i, point);
        }

        g.strokePath(p, juce::PathStrokeType(1));
    }

    void FFTComp::resized() {}

    void FFTComp::drawNextFrame(FFTData& data)
    {
        data.window.multiplyWithWindowingTable(data.fftData, data.fftSize);
        data.forwardFFT.performFrequencyOnlyForwardTransform(data.fftData);

        float min_dB = -72.f;
        float max_dB = 0;

        for (int i = 0; i < data.scopeSize; i++)
        {
            auto skewedProportionX = 1.0f - std::exp(std::log(1.0f - (float)i / (float)data.scopeSize));
            auto fftDataIndex = juce::jlimit(0, data.fftSize / 2, (int)(skewedProportionX * (float)data.fftSize * 0.5f));
            auto level = juce::jmap(juce::jlimit(min_dB, max_dB, juce::Decibels::gainToDecibels(data.fftData[fftDataIndex])
                - juce::Decibels::gainToDecibels((float)data.fftSize)),
                min_dB, max_dB, 0.0f, 1.0f);

            data.scopeData[i] = level;
        }
    }

    //=======================================FFT=======================================

    //=======================================Oscilloscope==============================

    OscilloscopeComp::OscilloscopeComp(OscilloscopeData& d)
        : data(d) {}

    OscilloscopeComp::~OscilloscopeComp() {}

    void OscilloscopeComp::paint(juce::Graphics& g)
    {
        auto bounds = getLocalBounds();
        auto height = bounds.getHeight();
        float width = bounds.getWidth();

        g.setColour(juce::Colours::black);

        juce::Path path;

        path.startNewSubPath(0, height / 2);

        auto readPointer = data.oscBuffer.getReadPointer(0);
        auto size = data.oscBuffer.getNumSamples();

        auto ratio = size / width;

        for (int point = 0; point < size; point += ratio)
        {
            audioPoints.push_back(readPointer[point]);
        }

        for (int i = 0; i < size; i++)
        {

            auto point = juce::jmap(readPointer[i], -1.f, 1.f, (float)height, 0.f);
            path.lineTo(i, point);
        }

        g.strokePath(path, juce::PathStrokeType(1));
    }

    void OscilloscopeComp::resized() {}

    OscilloscopeData::OscilloscopeData() {}

    OscilloscopeData::~OscilloscopeData() {}

    void OscilloscopeData::setBuffer(juce::AudioBuffer<float> buffer)
    {
        oscBuffer.setSize(2, buffer.getNumSamples());
        oscBuffer = buffer;
    }
