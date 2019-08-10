#include "signaldata.h"
#include <qvector.h>
#include <qmutex.h>
#include <qreadwritelock.h>

class SignalData::PrivateData
{
public:
    PrivateData():
        boundingRect( 1.0, 1.0, -2.0, -2.0 ) // invalid
    {
        values.reserve( 1000 );
    }

    inline void append( const QPointF &sample )
    {
        values.append( sample );

        // adjust the bounding rectangle

        if ( boundingRect.width() < 0 || boundingRect.height() < 0 )
        {
            boundingRect.setRect( sample.x(), sample.y(), 0.0, 0.0 );
        }
        else
        {
            boundingRect.setRight( sample.x() );


            if ( sample.y() < boundingRect.bottom() )
                boundingRect.setBottom( sample.y() );

            if ( sample.y() > boundingRect.top() )
                boundingRect.setTop( sample.y() );
        }
    }

    QReadWriteLock lock;

    QVector<QPointF> values;
    QRectF boundingRect;

    QMutex mutex; // protecting pendingValues
    QVector<QPointF> pendingValues;
};

SignalData::SignalData()
{
    d_data = new PrivateData();
}

SignalData::~SignalData()
{
    delete d_data;
}

int SignalData::size() const
{
    return d_data->values.size();
}

QPointF SignalData::value( int index ) const
{
    return d_data->values[index];
}

QRectF SignalData::boundingRect() const
{
    return d_data->boundingRect;
}

void SignalData::lock()
{
    d_data->lock.lockForRead();
}

void SignalData::unlock()
{
    d_data->lock.unlock();
}

void SignalData::append( const QPointF &sample )
{
    static QPointF last_sample(0.0,0.0);
    d_data->mutex.lock();
    d_data->pendingValues += sample;
    const bool isLocked = d_data->lock.tryLockForWrite();
    if ( isLocked )
    {
        const int numValues = d_data->pendingValues.size();
        const QPointF *pendingValues = d_data->pendingValues.data();

        for ( int i = 0; i < numValues; i++ )
        {

             d_data->append( pendingValues[i] );
        }
       // d_data->append(sample);

        d_data->pendingValues.clear();

        d_data->lock.unlock();
    }

    d_data->mutex.unlock();
}

void SignalData::clearStaleValues( double limit )
{

    d_data->lock.lockForWrite();

    d_data->boundingRect = QRectF( 1.0, 1.0, -2.0, -2.0 ); // invalid

    values.clear();
    values.reserve(1000);
    values = d_data->values;
    d_data->values.clear();
    d_data->values.reserve( 1000 );
    //int index=0;
    /*for ( index = values.size() - 1; index >= 0; index-- )
    {
        if ( values[index].x() < limit )
            break;
    }*/
   // if ( index > 0 )
    //    d_data->append( values[0] );

    /*while ( index < 999  )
    {
        d_data->append( values[index++] );
    }*/

    d_data->lock.unlock();
}

SignalData &SignalData::instance()
{
    static SignalData valueVector;
    return valueVector;
}


void SignalData::plotdata()
{
    d_data->lock.lockForWrite();
    int index = 0;
    for(index = 0;index<1000;index++)
    {
        if(values.size()>0)
        d_data->append( values[index] );
    }
    d_data->lock.unlock();
}
