#include <QCoreApplication>
#include <QtCore/QDebug>
#include <QSqlQuery>
#include <QRegExp>
#include <QDateTime>
#include <QSqlRecord>
#include <QSqlError>
#include <QSqlField>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <math.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QDateTime _begin_time, _end_time, _curr_time;
    _begin_time =  QDateTime::fromString(argv[2],  "yyyy-MM-ddTHH:mm:ss");
    _curr_time = _begin_time;
    _end_time = QDateTime::fromString(argv[3],  "yyyy-MM-ddTHH:mm:ss");
    QMap<QDateTime, float> *time_frame = new  QMap<QDateTime, float>;

    while (_curr_time < _end_time)
    {

        _curr_time = _curr_time.addSecs(1200);
        if (_curr_time < _end_time)
        {
            time_frame->insert(_curr_time, 0.0f);
        } else {
            time_frame->insert(_end_time, 0.0f);
        }

    }




    QTextStream(stdout) << "Parameters " << argv[1] <<" - - - "<< _begin_time.toString() << " - - - " << _end_time.toString() << " - - - " << argv[4]<< endl;

    const QStringList template_chemical = (QStringList() <<"NO" << "NO2" << "NH3"<< "SO2" << "H2S"<< "O3"<< "CO"<< "CH2O"<< "PM1"<< "PM2.5"<< "PM10"<< "Пыль общая"<< "бензол"<< "толуол"<< "этилбензол"<< "м,п-ксилол"<< "о-ксилол"<< "хлорбензол"<< "стирол"<< "фенол");

    const QStringList template_weather = (QStringList() << "Темп. внешняя" << "Направление ветра" << "Скорость ветра" << "Влажность внеш."  );


    QMap<QString, int> *chemical_classes = new  QMap<QString, int>;

    chemical_classes->insert("NO",3);
    chemical_classes->insert("NO2", 3);
    chemical_classes->insert("NH3", 4);
    chemical_classes->insert("SO2", 3);
    chemical_classes->insert("H2S", 3);
    chemical_classes->insert("O3", 1);
    chemical_classes->insert("CO", 4);
    chemical_classes->insert("CH2O", 2);
    chemical_classes->insert("PM1", 3);
    chemical_classes->insert("PM2.5", 3);
    chemical_classes->insert("PM10", 3);
    chemical_classes->insert("Пыль общая", 3);
    chemical_classes->insert("бензол", 2);
    chemical_classes->insert("толуол", 3);
    chemical_classes->insert("этилбензол", 4);
    chemical_classes->insert("м,п-ксилол", 3);
    chemical_classes->insert("о-ксилол", 3);
    chemical_classes->insert("хлорбензол", 3);
    chemical_classes->insert("стирол", 3);
    chemical_classes->insert("фенол", 2);


    QStringListIterator template_chemical_iterator(template_chemical);
    QStringListIterator template_weather_iterator(template_weather);

    QString _chemical;

    QMap<QString, QVariant> *jsonobj, * footer;
    QMap<QString, QMap<QString, QVariant> *> *chemical_by_day_chain;

    jsonobj= new  QMap<QString, QVariant>;
    footer = new QMap<QString, QVariant>;

    chemical_by_day_chain = new  QMap<QString, QMap<QString, QVariant>*>;

    while (template_chemical_iterator.hasNext())
    {


        _chemical = template_chemical_iterator.next();

        //_chemical_chain{{"срака овечья", 1}};


        //chemical_chain[_chemical1.trimmed()] = -1.0;
        jsonobj->insert(_chemical, -1.0);
        //jsonobj->insert(QString(_chemical).append("_range"), false);
        //jsonobj->insert(QString(_chemical).append("_empty"), false);

        footer->insert(QString(_chemical).append("_exceed1"), 0);
        footer->insert(QString(_chemical).append("_exceed5"), 0);
        footer->insert(QString(_chemical).append("_exceed10"), 0);
        footer->insert(QString(_chemical).append("_average"), -1.0);
        footer->insert(QString(_chemical).append("_count"), 0);
        footer->insert(QString(_chemical).append("_max"), 0.0);
        footer->insert(QString(_chemical).append("_time_max"), 0);
        footer->insert(QString(_chemical).append("_min"), -1.0);
        footer->insert(QString(_chemical).append("_time_min"), 0);
        footer->insert(QString(_chemical).append("_max_consentration"), -1.0);
        footer->insert(QString(_chemical).append("_time_max_consentration"), 0);
        footer->insert(QString(_chemical).append("_sindex"), -1.0);
        footer->insert(QString(_chemical).append("_greatest_repeatably"), -1.0);
        footer->insert(QString(_chemical).append("_sigma"), -1.0);


        //  chemical_chain.append(QJsonValue( {QString(_chemical).append("_range") , false}));
        // chemical_chain.append(QJsonValue( {QString(_chemical).append("_empty") , false}));

    }

    _curr_time = _begin_time;

    while (_curr_time < _end_time)
    {
        QMap<QString, QVariant> *jsonobj_cpy =  new QMap<QString, QVariant>(  *jsonobj);
        //memcpy(jsonobj_cpy, jsonobj, sizeof (*jsonobj));




        if (_curr_time < _end_time)
        {
            //  chemical_daily_chain[_curr_time.toString("yyyy-MM-ddTHH:mm:ss")] = chemical_chain;
            chemical_by_day_chain->insert(_curr_time.toString("yyyy-MM-dd"), jsonobj_cpy );
            //{"NO" , "NO2" , "NH3", "SO2" , "H2S", "O3", "CO", "CH2O", "PM1", "PM2.5", "PM10", "Пыль общая", "бензол", "толуол", "этилбензол", "м,п-ксилол", "о-ксилол", "хлорбензол", "стирол", "фенол" });

        }
        _curr_time = _curr_time.addDays(1);


    }

    //QVariant ooo = chemical_by_day_chain->value("01")->value("NO");


    QSqlDatabase * m_conn = new QSqlDatabase();
    *m_conn = QSqlDatabase::addDatabase("QPSQL");
    m_conn->setHostName("localhost");
    m_conn->setDatabaseName("weather");
    m_conn->setUserName("weather");
    m_conn->setPassword("31415");

    bool status = m_conn->open();
    if (!status)
    {
        //releaseModbus();

        QTextStream(stdout) << ( QString("Connection error: " + m_conn->lastError().text()).toLatin1().constData()) <<   " \n\r";
        return -1;

    }

    //range threshoulds load

    QMap<QString, float> *chemical_ranges = new  QMap<QString, float>;

    QSqlQuery *query_equipments= new QSqlQuery ("select * from equipments where is_present = 'true' and idd = '"+ QString(argv[1]) +"'", *m_conn);

    QSqlRecord rec;

    query_equipments->first();

    for (int i = 0; i < query_equipments->size(); i++ )

    {
        rec = query_equipments->record();
        if (rec.field("max_day_consentration").value().toFloat() >0)
            chemical_ranges->insert(rec.field("typemeasure").value().toString(),rec.field("max_day_consentration").value().toFloat());

        query_equipments->next();


    }
    query_equipments->finish();


    // macs table load
    QSqlQuery *query_macs= new QSqlQuery ("select * from macs", *m_conn);

    QMap<QString, float>  *m_macs =  new QMap<QString, float>;

    query_macs->first();

    for (int i = 0; i < query_macs->size(); i++ )
    {

        rec = query_macs->record();

        m_macs->insert(rec.field("chemical").value().toString(),rec.field("max_m").value().toFloat());
        query_macs->next();

    }

    query_macs->finish();



    //measure load
    QDateTime _tmp_dtime, min_dtime, max_dtime, min_macs_dtime, max_macs_dtime, day_current;
    QMap<QDateTime, float>::iterator _dtime_iterator;
    QSqlQuery *query;
    float _measure, _tmp_measure, min_measure, max_measure, min_macs, max_macs, total_sum, day_sum, standard_index, greatest_repeatably, sigma, sum_pow2,range;
    int _cnt, total_cnt, frames_cnt, day_counter, day_number;
    int local_counter_macs1, local_counter_macs5, local_counter_macs10;
    bool outranged = false;

    template_chemical_iterator.toFront();

    while (template_chemical_iterator.hasNext())
    {


        QString _chemical = template_chemical_iterator.next();
        query = new QSqlQuery ("select * from sensors_data where idd='" + QString(argv[1]) +"' and date_time >= '"+ QString(argv[2]) +"' and date_time < '"+
                QString(argv[3])+"' and typemeasure = '" + _chemical +"'order by date_time asc", *m_conn);
        query->first();

        total_sum = 0.0f;
        total_cnt = 0;
        frames_cnt = 0;
        day_counter = 0;
        day_sum = 0.0f;
        day_number  = 0;

        local_counter_macs1 = 0;
        local_counter_macs5 = 0;
        local_counter_macs10 = 0;

        min_macs = 1000000.0f;
        max_macs = -1.0f;
        min_measure = 1000000.0f;
        max_measure = -1.0f;

        range = chemical_ranges->value(_chemical);

        _dtime_iterator = time_frame->begin();
        day_current = _dtime_iterator.key();

        while (_dtime_iterator != time_frame->end()) {
            _measure = 0.0f;
            _cnt = 0;


            rec = query->record();

            _tmp_measure = (rec.field("measure").value().toFloat());
            _tmp_dtime = (rec.field("date_time").value().toDateTime());

            while ((_tmp_dtime < _dtime_iterator.key()) &&(_tmp_dtime.isValid())){

                _measure += _tmp_measure;
                _cnt++;
                day_sum += _tmp_measure;
                day_counter++;
                if(_tmp_measure > range)
                    outranged = true;
                //if (_tmp_measure > max_measure )
                //{
                //      max_measure = _tmp_measure;
                //    max_dtime =  _tmp_dtime;
                // }

                // if (_tmp_measure < min_measure )
                // {
                //    min_measure = _tmp_measure;
                //     min_dtime =  _tmp_dtime;
                // }
                if (!query->next())
                    break;
                rec = query->record();
                _tmp_measure = (rec.field("measure").value().toFloat());
                _tmp_dtime = (rec.field("date_time").value().toDateTime());


            }
            if (_cnt)
            {
                time_frame->insert(_dtime_iterator.key(), _measure/_cnt);

                //macs counter increment
                if (_dtime_iterator.value()/10 > m_macs->value(_chemical))
                { local_counter_macs10++;
                } else {
                    if (_dtime_iterator.value()/5 > m_macs->value(_chemical))
                    {
                        local_counter_macs5 ++;
                    } else {
                        if (_dtime_iterator.value() > m_macs->value(_chemical))
                            local_counter_macs1++;
                    }
                }

                if (_dtime_iterator.value() > max_macs)
                {
                    max_macs = _dtime_iterator.value();
                    max_macs_dtime = _dtime_iterator.key();
                }


                total_sum += _measure;

                total_cnt += _cnt;
                frames_cnt++;



            }
            _dtime_iterator++;
            if((day_current.daysTo(_dtime_iterator.key()) > 0)||(_dtime_iterator == time_frame->end()))
            {
                //chemical_chain[day_number].toObject().take(_chemical)

                QVariant op ;
                float _day_measure = (day_sum / day_counter);
                if (day_counter)
                    chemical_by_day_chain->value(day_current.toString("yyyy-MM-dd"))->insert(_chemical, _day_measure);

                if (_day_measure > max_measure )
                {
                    max_measure = _day_measure;
                    max_dtime =  day_current;
                }

                if (_day_measure < min_measure )
                {
                    min_measure = _day_measure;
                    min_dtime =  day_current;
                }

                if ((day_counter < 36) && day_counter) // if measure less than 1/2 quantity of 20-minutes inrervals per day
                {
                    chemical_by_day_chain->value(day_current.toString("yyyy-MM-dd"))->insert(QString(_chemical).append("_empty"), true);

                }

                if ((day_counter > 36))
                {
                    chemical_by_day_chain->value(day_current.toString("yyyy-MM-dd"))->insert(QString(_chemical).append("_empty"), false);

                }

                if (outranged && day_counter)
                    chemical_by_day_chain->value(day_current.toString("yyyy-MM-dd"))->insert(QString(_chemical).append("_range"), true);

                if (!outranged && day_counter)
                    chemical_by_day_chain->value(day_current.toString("yyyy-MM-dd"))->insert(QString(_chemical).append("_range"), false);

                if (local_counter_macs1)
                    chemical_by_day_chain->value(day_current.toString("yyyy-MM-dd"))->insert(QString(_chemical).append("_macs"), 1);

                if (local_counter_macs5)
                    chemical_by_day_chain->value(day_current.toString("yyyy-MM-dd"))->insert(QString(_chemical).append("_macs"), 5);

                if(local_counter_macs10)
                    chemical_by_day_chain->value(day_current.toString(""))->insert(QString(_chemical).append("_macs"), 10);



                day_current =_dtime_iterator.key();
                day_counter = 0;
                day_number++;
                day_sum = 0.0;
                outranged = false;
            }

        }
        // after each calculation by time frame
        if (total_cnt){
            _dtime_iterator = time_frame->begin();

            sum_pow2 = 0.0f;
            total_sum = total_sum / total_cnt; //new purpose is quotient of total_sum
            while (_dtime_iterator != time_frame->end()) {
                sum_pow2 += (_dtime_iterator.value() - total_sum)*(_dtime_iterator.value() - total_sum);
                _dtime_iterator++;
            }

            standard_index =(m_macs->value(_chemical) < 900) ? max_macs / m_macs->value(_chemical) : -1.0f;

            greatest_repeatably = (m_macs->value(_chemical) < 900) ? (local_counter_macs1 + local_counter_macs5 + local_counter_macs10)/(frames_cnt ) *100 : -1.0f;

            sigma = sqrt (sum_pow2 / frames_cnt);

            footer->insert(QString(_chemical).append("_exceed1"), local_counter_macs1);
            footer->insert(QString(_chemical).append("_exceed5"), local_counter_macs5);
            footer->insert(QString(_chemical).append("_exceed10"), local_counter_macs10);
            footer->insert(QString(_chemical).append("_average"), (total_sum));
            footer->insert(QString(_chemical).append("_count"), frames_cnt);
            footer->insert(QString(_chemical).append("_max"), max_measure);
            footer->insert(QString(_chemical).append("_time_max"), max_dtime.toString("dd-MM-yyyy"));
            footer->insert(QString(_chemical).append("_min"), min_measure);
            footer->insert(QString(_chemical).append("_time_min"), min_dtime.toString("dd-MM-yyyy"));
            footer->insert(QString(_chemical).append("_max_consentration"), max_macs);
            footer->insert(QString(_chemical).append("_time_max_consentration"), max_macs_dtime.addSecs(-60).toString("dd-MM-yyyy HH:mm:ss"));
            footer->insert(QString(_chemical).append("_sindex"), standard_index);
            footer->insert(QString(_chemical).append("_greatest_repeatably"), greatest_repeatably);
            footer->insert(QString(_chemical).append("_sigma"), sigma);

            if (local_counter_macs1)
                footer->insert(QString(_chemical).append("_classcss"), 1);
            if (local_counter_macs5)
                footer->insert(QString(_chemical).append("_classcss"), 5);

            if(local_counter_macs10)
                footer->insert(QString(_chemical).append("_classcss"), 10);

        } else {
            footer->insert(QString(_chemical).append("_exceed1"), -1);
            footer->insert(QString(_chemical).append("_exceed5"), -1);
            footer->insert(QString(_chemical).append("_exceed10"), -1);
            footer->insert(QString(_chemical).append("_max"), -1);
            footer->insert(QString(_chemical).append("_time_max"), -1);
            footer->insert(QString(_chemical).append("_time_min"), -1);
            footer->insert(QString(_chemical).append("_time_max_consentration"),-1);


        }
        query->finish();
        query->~QSqlQuery();

    }

    //weather calculation
    template_weather_iterator.toFront();

    while (template_weather_iterator.hasNext())
    {


        QString _chemical = template_weather_iterator.next();
        query = new QSqlQuery ("select * from sensors_data where idd='" + QString(argv[1]) +"' and date_time >= '"+ QString(argv[2]) +"' and date_time < '"+
                QString(argv[3])+"' and typemeasure = '" + _chemical +"'order by date_time asc", *m_conn);
        query->first();

        _measure = 0.0f;
        _cnt = 0;

        _dtime_iterator = time_frame->begin();
        day_current = _dtime_iterator.key();

        while (_dtime_iterator != time_frame->end()) {


            rec = query->record();

            _tmp_measure = (rec.field("measure").value().toFloat());
            _tmp_dtime = (rec.field("date_time").value().toDateTime());

            while ((_tmp_dtime < _dtime_iterator.key()) &&(_tmp_dtime.isValid())){

                _measure += _tmp_measure;
                _cnt++;


                if (!query->next())
                    break;
                rec = query->record();
                _tmp_measure = (rec.field("measure").value().toFloat());
                _tmp_dtime = (rec.field("date_time").value().toDateTime());


            }

            _dtime_iterator++;

            if((day_current.daysTo(_dtime_iterator.key()) > 0)||(_dtime_iterator == time_frame->end()))
            {

                float _day_measure = (_measure / _cnt);
                if (_cnt)
                    chemical_by_day_chain->value(day_current.toString("yyyy-MM-dd"))->insert(_chemical, _day_measure);


                day_current =_dtime_iterator.key();
                _measure = 0.0f;
                _cnt = 0;

            }


        }
    }
    //chemical_daily_chain[_chemical + "_standard_index"] = 0.0;
    //chemical_daily_chain[_chemical + "_greatest_repeatably"] = 0.0;
    //chemical_daily_chain[_chemical + "_sigma"] = 0.0;
    /*QSqlQuery *query= new QSqlQuery ("select * from sensors_data where idd='" + QString(argv[1]) +"' and date_time >= '"+ QString(argv[2]) +"' and date_time < '"+
            QString(argv[3])+"' order by date_time asc", *m_conn);
    query->first();

    float _measure;

    int _cnt =0;
    for (int i = 0; i < query->size(); i++ )
    {
        //qDebug() << query->value("typemeasure").toString() << "  -----  "<< query->value("serialnum").toUuid() <<"\n\r";

        // m_uuid->insert( query->value("typemeasure").toString(), query->value("serialnum").toUuid());
        rec = query->record();
        _measure = (rec.field("measure").value().toFloat());

        QTextStream(stdout)<< i << "   " <<rec.field("date_time").value().toString() <<"- - - " << rec.field("typemeasure").value().toString() << " - - - "<< _measure<< endl;
        _cnt++;
        query->next();

    }
    query->finish();

    QTextStream(stdout) <<"Total are "<< _cnt<<" records" << endl;*/

    QFile file("./api/monthly.csv");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return -1;

    QTextStream out(&file);
    QMap<QString, QMap<QString, QVariant> *>::iterator _i;


    out << "date;";

    template_weather_iterator.toFront();

    while (template_weather_iterator.hasNext())
    {
        QString _chemical = template_weather_iterator.next();

        out << _chemical << ";";

    }

    template_chemical_iterator.toFront();

    while (template_chemical_iterator.hasNext())
    {
        QString _chemical = template_chemical_iterator.next();

        out << _chemical << ";";

    }
    out << "\n";

    for ( _i = chemical_by_day_chain->begin(); _i != chemical_by_day_chain->end();++_i)

    {
        template_weather_iterator.toFront();
        //out << QString(_i.key()).append("_date") << ";";
        out <<_i.key() <<";";

        //weather out
        while (template_weather_iterator.hasNext())
        {
            QString _chemical = template_weather_iterator.next();

            QList< QVariant> _jsonobj_cpy =  _i.value()->values(_chemical);
            if (_jsonobj_cpy.size() == 0)
            {
                out <<  "-;";
            } else {
                out << _jsonobj_cpy.first().toString() << ";";

            }

        }
        // measure out
       template_chemical_iterator.toFront();

        while (template_chemical_iterator.hasNext())
        {
            QString _chemical = template_chemical_iterator.next();
            QList< QVariant> _jsonobj_cpy =  _i.value()->values(_chemical);
            if (_jsonobj_cpy.first().toInt() == -1)
            {
                out <<  "-;";
            } else {
                out << _jsonobj_cpy.first().toString() << ";";

            }

        }
        out << "\n";

        //errors out

        template_chemical_iterator.toFront();
        out << _i.key()<< "_empty;;;;;";

        //template_weather_iterator.toFront();

        //weather empty out
        /*while (template_weather_iterator.hasNext())
        {
            QString _chemical = template_weather_iterator.next();

            QList< QVariant> _jsonobj_cpy =  _i.value()->values(_chemical);
            if (_jsonobj_cpy.size() == 0)
            {
                out <<  "-;";
            } else {
                out << _jsonobj_cpy.first().toString() << ";";

            }

        }*/

        //measures empty out

        while (template_chemical_iterator.hasNext())
        {
            QString _chemical = template_chemical_iterator.next();
            QList< QVariant> _jsonobj_cpy =  _i.value()->values(QString(_chemical).append("_empty"));
            if (_jsonobj_cpy.size() > 0) {
                if (!_jsonobj_cpy.first().toBool())
                {
                    out <<  "false;";
                } else {
                    out <<  "true;";

                }
            } else {
                out << "-;";
            }

        }
        out << "\n";

        //outranges out
        template_chemical_iterator.toFront();
        out << _i.key()<< "_outrange;;;;;";

        while (template_chemical_iterator.hasNext())
        {
            QString _chemical = template_chemical_iterator.next();
            QList< QVariant> _jsonobj_cpy =  _i.value()->values(QString(_chemical).append("_range"));
            if (_jsonobj_cpy.size() > 0) {
                if (!_jsonobj_cpy.first().toBool())
                {
                    out <<  "false;";
                } else {
                    out <<  "true;";

                }
            } else {
                out << "-;";
            }


        }
        out << "\n";

        //macs css out
        template_chemical_iterator.toFront();
        out << _i.key()<< "_macs;;;;;";

        while (template_chemical_iterator.hasNext())
        {
            QString _chemical = template_chemical_iterator.next();
            QList< QVariant> _jsonobj_cpy =  _i.value()->values(QString(_chemical).append("_macs"));
            if (_jsonobj_cpy.size() > 0) {

                    out <<  _jsonobj_cpy.first().toString() << ";";

            }

        }
        out << "\n";

    }



    //footer

    template_chemical_iterator.toFront();
    out << "average" << ";;;;;";
    while (template_chemical_iterator.hasNext())
    {
        QString _chemical = template_chemical_iterator.next();
        QList< QVariant> _footer = footer->values(QString(_chemical).append("_average"));

        footer->values(QString(_chemical).append("_average")).value(0).toInt() == -1 ? out <<  "-;" : out << footer->values(QString(_chemical).append("_average")).value(0).toString() << ";";

    }
    out << "\n";
    template_chemical_iterator.toFront();
    out << "count" << ";;;;;";
    while (template_chemical_iterator.hasNext())
    {
        QString _chemical = template_chemical_iterator.next();
        QList< QVariant> _footer = footer->values(QString(_chemical).append("_count"));

        footer->values(QString(_chemical).append("_count")).value(0).toInt() == 0 ? out <<  "-;" : out << footer->values(QString(_chemical).append("_count")).value(0).toString() << ";";
    }
    out << "\n";
    template_chemical_iterator.toFront();
    out << "max_measure" << ";;;;;";
    while (template_chemical_iterator.hasNext())
    {
        QString _chemical = template_chemical_iterator.next();
        QList< QVariant> _footer = footer->values(QString(_chemical).append("_max"));
        footer->values(QString(_chemical).append("_max")).value(0).toInt() == -1 ? out <<  "-;" : out << footer->values(QString(_chemical).append("_max")).value(0).toString() << ";";
    }
    out << "\n";
    template_chemical_iterator.toFront();
    out << "max_time" << ";;;;;";
    while (template_chemical_iterator.hasNext())
    {
        QString _chemical = template_chemical_iterator.next();
        QList< QVariant> _footer = footer->values(QString(_chemical).append("_time_max"));
        footer->values(QString(_chemical).append("_time_max")).value(0).toInt() == -1 ? out <<  "-;" : out << footer->values(QString(_chemical).append("_time_max")).value(0).toString() << ";";
    }
    out << "\n";
    template_chemical_iterator.toFront();
    out << "min_measure" << ";;;;;";
    while (template_chemical_iterator.hasNext())
    {
        QString _chemical = template_chemical_iterator.next();
        QList< QVariant> _footer = footer->values(QString(_chemical).append("_min"));
        footer->values(QString(_chemical).append("_min")).value(0).toInt() == -1 ? out <<  "-;" : out << footer->values(QString(_chemical).append("_min")).value(0).toString() << ";";
    }
    out << "\n";
    template_chemical_iterator.toFront();
    out << "min_time" << ";;;;;";
    while (template_chemical_iterator.hasNext())
    {
        QString _chemical = template_chemical_iterator.next();
        QList< QVariant> _footer = footer->values(QString(_chemical).append("_time_min"));
        footer->values(QString(_chemical).append("_time_min")).value(0).toInt() == -1 ? out <<  "-;" : out << footer->values(QString(_chemical).append("_time_min")).value(0).toString() << ";";
    }
    out << "\n";
    template_chemical_iterator.toFront();
    out << "max_concentration_measure" << ";;;;;";
    while (template_chemical_iterator.hasNext())
    {
        QString _chemical = template_chemical_iterator.next();
        QList< QVariant> _footer = footer->values(QString(_chemical).append("_max_consentration"));
        footer->values(QString(_chemical).append("_max_consentration")).value(0).toInt() == -1 ? out <<  "-;" : out << footer->values(QString(_chemical).append("_max_consentration")).value(0).toString() << ";";
    }
    out << "\n";
    template_chemical_iterator.toFront();
    out << "time_max_concentration" << ";;;;;";
    while (template_chemical_iterator.hasNext())
    {
        QString _chemical = template_chemical_iterator.next();
        QList< QVariant> _footer = footer->values(QString(_chemical).append("_time_max_consentration"));
        footer->values(QString(_chemical).append("_time_max_consentration")).value(0).toInt() == -1 ? out <<  "-;" : out << footer->values(QString(_chemical).append("_time_max_consentration")).value(0).toString() << ";";
    }
    out << "\n";
    template_chemical_iterator.toFront();
    out << "exceed_1" << ";;;;;";
    while (template_chemical_iterator.hasNext())
    {
        QString _chemical = template_chemical_iterator.next();
        QList< QVariant> _footer = footer->values(QString(_chemical).append("_exceed1"));
        footer->values(QString(_chemical).append("_exceed1")).value(0).toInt() == -1 ? out <<  "-;" : out << footer->values(QString(_chemical).append("_exceed1")).value(0).toString() << ";";
    }
    out << "\n";
    template_chemical_iterator.toFront();
    out << "exceed5" << ";;;;;";
    while (template_chemical_iterator.hasNext())
    {
        QString _chemical = template_chemical_iterator.next();
        QList< QVariant> _footer = footer->values(QString(_chemical).append("_exceed5"));
        footer->values(QString(_chemical).append("_exceed5")).value(0).toInt() == -1 ? out <<  "-;" : out << footer->values(QString(_chemical).append("_exceed5")).value(0).toString() << ";";
    }
    out << "\n";
    template_chemical_iterator.toFront();
    out << "exceed10" << ";;;;;";
    while (template_chemical_iterator.hasNext())
    {
        QString _chemical = template_chemical_iterator.next();
        QList< QVariant> _footer = footer->values(QString(_chemical).append("_exceed10"));
        footer->values(QString(_chemical).append("_exceed10")).value(0).toInt() == -1 ? out <<  "-;" : out << footer->values(QString(_chemical).append("_exceed10")).value(0).toString() << ";";
    }
    out << "\n";
    template_chemical_iterator.toFront();
    out << "sindex" << ";;;;;";
    while (template_chemical_iterator.hasNext())
    {
        QString _chemical = template_chemical_iterator.next();
        QList< QVariant> _footer = footer->values(QString(_chemical).append("_sindex"));
        footer->values(QString(_chemical).append("_sindex")).value(0).toInt() == -1 ? out <<  "-;" : out << footer->values(QString(_chemical).append("_sindex")).value(0).toString() << ";";
    }
    out << "\n";

    template_chemical_iterator.toFront();
    out << "repeatably" << ";;;;;";
    while (template_chemical_iterator.hasNext())
    {
        QString _chemical = template_chemical_iterator.next();
        QList< QVariant> _footer = footer->values(QString(_chemical).append("_greatest_repeatably"));
        footer->values(QString(_chemical).append("_greatest_repeatably")).value(0).toInt() == -1 ? out <<  "-;" : out << footer->values(QString(_chemical).append("_greatest_repeatably")).value(0).toString() << ";";
    }
    out << "\n";

    template_chemical_iterator.toFront();
    out << "sigma" << ";;;;;";
    while (template_chemical_iterator.hasNext())
    {
        QString _chemical = template_chemical_iterator.next();
        QList< QVariant> _footer = footer->values(QString(_chemical).append("_sigma"));
        footer->values(QString(_chemical).append("_sigma")).value(0).toInt() == -1 ? out <<  "-;" : out << footer->values(QString(_chemical).append("_sigma")).value(0).toString() << ";";
    }
    out << "\n";

    template_chemical_iterator.toFront();
    out << "classcss" << ";;;;;";
    while (template_chemical_iterator.hasNext())
    {
        QString _chemical = template_chemical_iterator.next();
        QList< QVariant> _footer = footer->values(QString(_chemical).append("_classcss"));
         out << footer->values(QString(_chemical).append("_classcss")).value(0).toString() << ";";
    }
    out << "\n";

    file.close();

    return 0;
}
