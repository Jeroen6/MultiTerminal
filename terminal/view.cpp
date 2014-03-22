#include "view.h"
#include "ui_view.h"

View::View(int argViewID, QString argFilter, QWidget *parent):
    QMainWindow(parent),
    ui(new Ui::View)
{
    ViewID = argViewID;
    filter = argFilter;

    ui->setupUi(this);
}

View::~View()
{
    delete ui;
}

void View::on_actionClose_triggered()
{
    this->close();
}

void View::write(QString data){
    if(data != ""){
        data.replace(QChar('\n'),QString("<font color=\"red\">\\n<\font>"));
        data.replace(QChar('\r'),QString("<font color=\"red\">\\r<\font>"));
        QCursor c = ui->textInput->cursor();
        ui->textInput->append(data);
        if(ui->checkAutoScroll->checkState()==Qt::Checked)
            ui->textInput->moveCursor(QTextCursor::End);
        else
            ui->textInput->setCursor(c);
    }

    /*
    ui->textInput->moveCursor(QTextCursor::End);
    ui->textInput->insertPlainText(data);
    ui->textInput->moveCursor(QTextCursor::End);
    */
}

void View::on_checkAutoScroll_stateChanged(int arg1)
{
    if(ui->checkAutoScroll->checkState()==Qt::Unchecked)
        ui->textInput->moveCursor(QTextCursor::WordLeft);

}

void View::on_buttonWipe_clicked()
{
    ui->textInput->clear();
}

void View::closeEvent( QCloseEvent * event ){
    closed(this);
}
