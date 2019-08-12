#include "samplingthread.h"
#include "signaldata.h"
#include <qwt_math.h>
#include <math.h>
bool plotcomplete;
bool ReadDataisOK = true;
#if QT_VERSION < 0x040600
#define qFastSin(x) ::sin(x)
#endif

SamplingThread::SamplingThread( QObject *parent ):
    QwtSamplingThread( parent ),
    d_frequency( 5.0 ),
    d_amplitude( 20.0 )
{
}

void SamplingThread::setFrequency( double frequency )
{
    d_frequency = frequency;
}

double SamplingThread::frequency() const
{
    return d_frequency;
}

void SamplingThread::setAmplitude( double amplitude )
{
    d_amplitude = amplitude;
}

double SamplingThread::amplitude() const
{
    return d_amplitude;
}

void SamplingThread::sample( double elapsed )
{
    //QPointF plotdata[1000];
    if ( d_frequency > 0.0&&plotcomplete == true)
    {
        plotcomplete = false;
        ReadDataisOK = false;
        SignalData::instance().lock();
        SignalData::instance().values.reserve(1000);
        SignalData::instance().clearStaleValues(0.001);
        for(int i=0;i<1000;i++)
        {
            double value = d_amplitude * qFastSin( (double)i/1000* d_frequency * 2 * M_PI );
            const QPointF s( (double)i/100, value );
            SignalData::instance().append( s );
        }
        ReadDataisOK = true;
        SignalData::instance().unlock();
    }
}

double SamplingThread::value( double timeStamp ) const
{
    const double period = 1.0 / d_frequency;

    const double x = ::fmod( timeStamp, period );
    const double v = d_amplitude * qFastSin( x / period * 2 * M_PI );

    return v;
}
