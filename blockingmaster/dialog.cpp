/****************************************************************************
**
** Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtSerialPort module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "dialog.h"

#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSerialPortInfo>
#include <QSpinBox>
#include <QCheckBox>
#include <QRadioButton>
#include <QGroupBox>


Dialog::Dialog(QWidget *parent) :
    //Init all private attributes before class instanciation
    QDialog(parent),
    m_serialPortLabel(new QLabel(tr("Puerto Serie:"))),
    m_serialPortComboBox(new QComboBox),
    m_waitResponseLabel(new QLabel(tr("Tiempo de Respuesta, msec:"))),
    m_waitResponseSpinBox(new QSpinBox),
    m_requestLabel(new QLabel(tr("Mensaje/Consigna:"))),
    m_requestLineEdit(new QLineEdit(tr(":xAAAAFFF"))),
    m_trafficLabel(new QLabel(tr("No hay mensajes."))),
    m_statusLabel(new QLabel(tr("Estado: Esperando."))),
    m_runButton(new QPushButton(tr("Enviar"))),
    //Add mauric -- #TODO remove this comment on the future
    m_frecuenciaLabel(new QLabel(tr("Frecuencia"))),
    m_frecuenciaInput(new QSpinBox),
    m_amplitudLabel(new QLabel(tr("Amplitud"))),
    m_amplitudInput(new QSpinBox),
    m_radio1(new QRadioButton(tr("Onda Cuadrada"))),
    m_radio2(new QRadioButton(tr("Onda Senoidal")))

   //--
{
    //Get all available serial port information
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos)
        m_serialPortComboBox->addItem(info.portName());

    //Set Spin box Range and Value of time response
    m_waitResponseSpinBox->setRange(0, 10000);
    m_waitResponseSpinBox->setValue(1000);

    // #TODO se pueden poner estos valores asociados a CONSTANTES #DEFINE
    m_frecuenciaInput->setRange(1,1000);
    m_frecuenciaInput->setValue(1);
    m_amplitudInput->setRange(0,60);
    m_amplitudInput->setValue(40);



    //Creaci'on de la interfaz grafica
    //Ubicacion en el Layout de los botones
    auto mainLayout = new QGridLayout;
    mainLayout->addWidget(m_serialPortLabel, 0, 0);
    mainLayout->addWidget(m_serialPortComboBox, 0, 1);
    mainLayout->addWidget(m_waitResponseLabel, 1, 0);
    mainLayout->addWidget(m_waitResponseSpinBox, 1, 1);
    mainLayout->addWidget(m_runButton, 12, 2, 2, 3);
    mainLayout->addWidget(m_trafficLabel, 10, 0, 1, 4);
    mainLayout->addWidget(m_statusLabel, 11, 0, 1, 5);
    mainLayout->addWidget(m_frecuenciaLabel,5,0);
    mainLayout->addWidget(m_frecuenciaInput,5,1);
    mainLayout->addWidget(m_amplitudLabel,6,0);
    mainLayout->addWidget(m_amplitudInput,6,1);
    m_exclusiveGroup = generarExclusiveGroup();
    mainLayout->addWidget(m_exclusiveGroup,7,0);
//    mainLayout->addWidget(m_ondaSenoCheck,7,0);
//    mainLayout->addWidget(m_ondaCuadradaCheck,8,0);
    mainLayout->addWidget(m_requestLabel, 9, 0);
    mainLayout->addWidget(m_requestLineEdit, 9, 1, 1, 3);
    setLayout(mainLayout);

    setWindowTitle(tr("IMERIS MesVib Comunication"));
    resize(480, 320);
    m_serialPortComboBox->setFocus();//Apuntar el cursor en serial port

    //Conectar los botones
//      connect(m_runButton, &QPushButton::clicked, this, &Dialog::transaction);
 //   connect(&m_thread, &MasterThread::response, this, &Dialog::showResponse);
 //   connect(&m_thread, &MasterThread::error, this, &Dialog::processError);
 //   connect(&m_thread, &MasterThread::timeout, this, &Dialog::processTimeout);
 
 // --------- qt 4 for RPI --------- //

    connect(m_runButton, SIGNAL(clicked()), this, SLOT(transaction()));
    connect(&m_thread, SIGNAL(response(QString)), this, SLOT(showResponse(QString)));
    connect(&m_thread, SIGNAL(error(QString)), this, SLOT(processError(QString)));
    connect(&m_thread, SIGNAL(timeout(QString)), this, SLOT(processTimeout(QString)));
 


}

QGroupBox *Dialog::generarExclusiveGroup()
{
    //Crear grupo de seleccion exclusiva

    QGroupBox *groupBox = new QGroupBox(tr("Tipo de onda"));

//     QCheckBox *radio1 = new QCheckBox(tr("Onda cuadrada"));
//    QCheckBox *radio2 = new QCheckBox(tr("Onda sin"));

    m_radio1->setChecked(true);
    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(m_radio1);
    vbox->addWidget(m_radio2);
    vbox->addStretch(1);
    groupBox->setLayout(vbox);

    return groupBox;

}

void Dialog::transaction()
{
    //Cuando da lugar a una transacci'on apaga la interfaz grafica
    setControlsEnabled(false);
    m_statusLabel->setText(tr("Estado: Corriendo,conectado al puerto %1.")
                           .arg(m_serialPortComboBox->currentText())); //inline string <- nombre del puerto


    //Add mauric
    //ensambla el mensaje antes de iniciar una transaccion
    mesvibMessage(m_frecuenciaInput->text(),m_amplitudInput->text());
    //--



    //Lanzamiento transaccion: NOMBRE PUERTO, TIEMPO RESPUESTA, MENSAJE
    m_thread.transaction(m_serialPortComboBox->currentText(),
                         m_waitResponseSpinBox->value(),
                         m_requestLineEdit->text());

}

void Dialog::showResponse(const QString &s)
{
    setControlsEnabled(true);
    m_trafficLabel->setText(tr("Mensaje enviado N: #%1:"
                               "\n\r-Mensaje: %2"
                               "\n\r-Verificacion: %3")
                            .arg(++m_transactionCount)
                            .arg(m_requestLineEdit->text())
                            .arg(s));
}

void Dialog::processError(const QString &s)
{
    setControlsEnabled(true);
    m_statusLabel->setText(tr("Estado: En espera, no mensaje, %1.").arg(s));
    m_trafficLabel->setText(tr("No hay mensajes."));
}

void Dialog::processTimeout(const QString &s)
{
    setControlsEnabled(true);
    m_statusLabel->setText(tr("Estado: Corriendo, %1.").arg(s));
    m_trafficLabel->setText(tr("Pero sin Mensajes."));
}

void Dialog::setControlsEnabled(bool enable)
{
    m_runButton->setEnabled(enable);
    m_serialPortComboBox->setEnabled(enable);
    m_waitResponseSpinBox->setEnabled(enable);
    m_requestLineEdit->setEnabled(enable);
}
//add mauric

void Dialog::mesvibMessage(const QString &amplitud, const QString &frecuencia)
{
    //FORMATO DE MENSAJE
    //:sAAAAFFF   onda senoidal
    //:cAAAAFFF   onda cuadrada
    QString tipo;
    QString amplitud_ = amplitud.rightJustified(4,'0');
    QString frecuencia_ = frecuencia.rightJustified(3,'0');

   //no recuperar los botenes sino declarlos dentro de esta clase
    if(m_radio1->isChecked())
    {
    //check onda cuadrada
    tipo = "c";
    m_requestLineEdit->setText(":"+tipo+amplitud_+frecuencia_);
    }
    else if(m_radio2->isChecked())
    {
    tipo = "s";
    m_requestLineEdit->setText(":"+tipo+amplitud_+frecuencia_);
    }
    // amplitud.

    //Inicializar String compartida con masterthread o crear

    //agregar token inicial


    //checkear tipo de onda y agregar token tipo de onda

    //checkear valores de amplitud y frecuencia

    // si se sarpan devolverlos

    //concatenar amplitud y frecuencia

    // verificar

    //concatenar con tipo de onda

    //mostrar en un cuadro o imprimirlo en la consola etc ....


}
