#ifndef _SIGNAL_DATA_H_
#define _SIGNAL_DATA_H_ 1

#include <qrect.h>
#include <qvector.h>

class SignalData
{
public:
    static SignalData &instance();

    void append( const QPointF &pos );
    void clearStaleValues( double min );
    void plotdata();

    int size() const;
    QPointF value( int index ) const;
    QVector<QPointF> values;
    QVector<QPointF> values_buffer;
    QRectF boundingRect() const;

    void lock();
    void unlock();
    void lockForWrite();

private:
    SignalData();
    SignalData( const SignalData & );
    SignalData &operator=( const SignalData & );

    virtual ~SignalData();

    class PrivateData;
    PrivateData *d_data;
};

#endif
