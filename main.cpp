#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QThread>
#include <QAbstractTableModel>
#include <algorithm>
using namespace std;

class TestModel : public QAbstractTableModel {
private:
    QList<QPair<int, QString>> rows;

public:
    TestModel() {}

    void addRows(QList<QPair<int, QString>> newRows) {
        beginInsertRows(QModelIndex(), rows.size(), rows.size() + newRows.size());
        rows.append(newRows);
        endInsertRows();
    }

    void removeSomeRows(int from, int to) {
        beginRemoveRows(QModelIndex(), from, to);
        int count = 0;
        while (count < to - from) {
            rows.remove(from);
            count++;
        }
        endRemoveRows();
    }

    void removeAllRows() {
        removeSomeRows(0, rows.size() - 1);
    }

    int columnCount(const QModelIndex& index) const override {
        return 2;
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override {
        return QString(section == 0 ? "Some num" : "Some string");
    }

    QVariant data(const QModelIndex& index, int role) const override {
        if (index.row() < 0 || index.row() >= rows.size()) {
            return QVariant();
        }
        if (index.column() >= 2) {
            return QVariant();
        }
        if (role == Qt::ItemDataRole::DisplayRole) {
            return index.column() == 0 ? QVariant::fromValue(rows.at(index.row()).first) : QVariant::fromValue(rows.at(index.row()).second);
        }
        return QVariant();
    }

    int rowCount(const QModelIndex& index) const override {
        return max(rows.size(), 10LL);
    }
};

class Controller : public QObject {
    Q_OBJECT
    Q_PROPERTY(TestModel* tableModel READ getModel CONSTANT)
private:
    TestModel* model;
public: Controller() {
        model = new TestModel();
        model->addRows(QList<QPair<int, QString>>() << QPair<int, QString>(12, "test one") <<  QPair<int, QString>(12, "test one")<< QPair<int, QString>(33, "test two")
                        << QPair<int, QString>(79, "test three") << QPair<int, QString>(45, "test four"));
        model->addRows(QList<QPair<int, QString>>() << QPair<int, QString>(12, "test one") << QPair<int, QString>(12, "test one")
                       << QPair<int, QString>(33, "test two") << QPair<int, QString>(79, "test three") << QPair<int, QString>(45, "test four") );
    }

    TestModel* getModel() { return model; }

    Q_INVOKABLE void addOtherThread() {
        qDebug() << "add Other Thread called";
        QThread* test = QThread::create( std::function<void ()> ([&]()  {
             qDebug() << "calling model add rows";
             // !!! here it crashes
             model->addRows(QList<QPair<int, QString>>() << QPair<int, QString>(12, "other thread one") << QPair<int, QString>(12, "other thread two"));
            qDebug() << "model add rows called";
        }));
        test->start();
    }
};

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(u"qrc:/test/main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.rootContext()->setContextProperty("controller", new Controller);
    engine.load(url);

    return app.exec();
}

#include "main.moc"
