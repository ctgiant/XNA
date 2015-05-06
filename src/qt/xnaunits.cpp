#include "xnaunits.h"

#include <QStringList>

DeOxyRiboseUnits::DeOxyRiboseUnits(QObject *parent):
        QAbstractListModel(parent),
        unitlist(availableUnits())
{
}

QList<DeOxyRiboseUnits::Unit> DeOxyRiboseUnits::availableUnits()
{
    QList<DeOxyRiboseUnits::Unit> unitlist;
    unitlist.append(XNA);
    unitlist.append(mXNA);
    unitlist.append(uXNA);
    return unitlist;
}

bool DeOxyRiboseUnits::valid(int unit)
{
    switch(unit)
    {
    case TXnA:
    case XNA:
    case dXNA:
    case mmXNA:
    case mXNA:
    case uXNA:
        return true;
    default:
        return false;
    }
}

QString DeOxyRiboseUnits::name(int unit)
{
    switch(unit)
    {
    case TXnA: return QString("TXnA");
    case XNA: return QString("XNA");
    case dXNA: return QString("dXNA");
    case mmXNA: return QString("mmXNA");
    case mXNA: return QString("mXNA");
    case uXNA: return QString::fromUtf8("μXNA");
    default: return QString("???");
    }
}

QString DeOxyRiboseUnits::description(int unit)
{
    switch(unit)
    {
    case TXnA: return QString("TonXNA (1 * 100)");
    case XNA: return QString("XNA)");
    case dXNA: return QString("Deci-XNA (1 / 10)");
    case mmXNA: return QString("Micro-XNA (1 / 100)");
    case mXNA: return QString("Milli-XNA (1 / 1,000)");
    case uXNA: return QString("Uni-XNA (1 / 1,000,000)");
    default: return QString("???");
    }
}

qint64 DeOxyRiboseUnits::factor(int unit)
{
    switch(unit)
    {
    case TXnA:  return  10000000000;
	case XNA:  return   1000000000;
	case dXNA:  return  100000000;
    case XNA:  return   1000000;
    case mXNA: return   1000;
    case uXNA: return   1;
    default:   return   1000000;
    }
}

int DeOxyRiboseUnits::amountDigits(int unit)
{
    switch(unit)
    {
    case TXnA: return  1; // 21,000,000 (# digits, without commas)
    case XNA: return   6; // 21,000,000,000
    case dXNA: return  11; // 21,000,000,000,000
    case mmXNA: return 15; // 21,000,000 (# digits, without commas)
    case mXNA: return  18; // 21,000,000,000
    case uXNA: return  20; // 21,000,000,000,000
    default: return 0;
    }
}

int DeOxyRiboseUnits::decimals(int unit)
{
    switch(unit)
    {
    case TXnA: return 15;
    case XNA: return 12;
    case dXNA: return 9;
    case mmXNA: return 6;
    case mXNA: return 3;
    case uXNA: return 0;
    default: return 0;
    }
}

QString DeOxyRiboseUnits::format(int unit, qint64 n, bool fPlus)
{
    // Note: not using straight sprintf here because we do NOT want
    // localized number formatting.
    if(!valid(unit))
        return QString(); // Refuse to format invalid unit
    qint64 coin = factor(unit);
    int num_decimals = decimals(unit);
    qint64 n_abs = (n > 0 ? n : -n);
    qint64 quotient = n_abs / coin;
    qint64 remainder = n_abs % coin;
    QString quotient_str = QString::number(quotient);
    QString remainder_str = QString::number(remainder).rightJustified(num_decimals, '0');

    // Right-trim excess zeros after the decimal point
    int nTrim = 0;
    for (int i = remainder_str.size()-1; i>=2 && (remainder_str.at(i) == '0'); --i)
        ++nTrim;
    remainder_str.chop(nTrim);

    if (n < 0)
        quotient_str.insert(0, '-');
    else if (fPlus && n > 0)
        quotient_str.insert(0, '+');
    return quotient_str + QString(".") + remainder_str;
}

QString DeOxyRiboseUnits::formatWithUnit(int unit, qint64 amount, bool plussign)
{
    return format(unit, amount, plussign) + QString(" ") + name(unit);
}

bool DeOxyRiboseUnits::parse(int unit, const QString &value, qint64 *val_out)
{
    if(!valid(unit) || value.isEmpty())
        return false; // Refuse to parse invalid unit or empty string
    int num_decimals = decimals(unit);
    QStringList parts = value.split(".");

    if(parts.size() > 2)
    {
        return false; // More than one dot
    }
    QString whole = parts[0];
    QString decimals;

    if(parts.size() > 1)
    {
        decimals = parts[1];
    }
    if(decimals.size() > num_decimals)
    {
        return false; // Exceeds max precision
    }
    bool ok = false;
    QString str = whole + decimals.leftJustified(num_decimals, '0');

    if(str.size() > 18)
    {
        return false; // Longer numbers will exceed 63 bits
    }
    qint64 retvalue = str.toLongLong(&ok);
    if(val_out)
    {
        *val_out = retvalue;
    }
    return ok;
}

int DeOxyRiboseUnits::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return unitlist.size();
}

QVariant DeOxyRiboseUnits::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if(row >= 0 && row < unitlist.size())
    {
        Unit unit = unitlist.at(row);
        switch(role)
        {
        case Qt::EditRole:
        case Qt::DisplayRole:
            return QVariant(name(unit));
        case Qt::ToolTipRole:
            return QVariant(description(unit));
        case UnitRole:
            return QVariant(static_cast<int>(unit));
        }
    }
    return QVariant();
}
