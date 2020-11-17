//
// Created by chudonghao on 2020/11/17.
//

#include "MainWindow.h"

#include <FastNoiseLite.h>

#include "ui_MainWindow.h"

Q_DECLARE_METATYPE(FastNoiseLite::NoiseType)

MainWindow::MainWindow(QWidget *parent) : Super(parent) {
  ui = new Ui;
  ui->setupUi(this);

  // noise type
  std::map<QString, FastNoiseLite::NoiseType> name2NoiseTypeMap = {
    {"Perlin", FastNoiseLite::NoiseType::NoiseType_Perlin},
    {"Cellular", FastNoiseLite::NoiseType::NoiseType_Cellular},
    {"OpenSimplex2", FastNoiseLite::NoiseType::NoiseType_OpenSimplex2},
    {"OpenSimplex2S", FastNoiseLite::NoiseType::NoiseType_OpenSimplex2S},
    {"Value", FastNoiseLite::NoiseType::NoiseType_Value},
    {"ValueCubic", FastNoiseLite::NoiseType::NoiseType_ValueCubic},
  };
  for (auto &p : name2NoiseTypeMap) {
    ui->noiseTypeComboBox->addItem(p.first, p.second);
  }
  ui->noiseTypeComboBox->setCurrentText("Perlin");

  // rotation type 3D
  std::map<QString, FastNoiseLite::RotationType3D> name2RotationType3DMap = {
    {"None", FastNoiseLite::RotationType3D::RotationType3D_None},
    {"ImproveXYPlanes", FastNoiseLite::RotationType3D::RotationType3D_ImproveXYPlanes},
    {"ImproveXZPlanes", FastNoiseLite::RotationType3D::RotationType3D_ImproveXZPlanes},
  };
  for (auto &p : name2RotationType3DMap) {
    ui->rotationType3DComboBox->addItem(p.first, p.second);
  }
  ui->rotationType3DComboBox->setCurrentText("None");

  // signals
  connect(ui->noiseCanvasWidget, &NoiseCanvasWidget::previewChanged, this, &Self::on_NoiseCanvas_PreviewChanged);

  connect(ui->scrollButton, &QPushButton::clicked, ui->noiseCanvasWidget, &NoiseCanvasWidget::setScroll);
  connect(ui->n3DCheckBox, &QCheckBox::stateChanged, this, &Self::generate);

  connect(ui->noiseTypeComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &Self::generate);
  connect(ui->rotationType3DComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &Self::generate);

  connect(ui->generateButton, &QPushButton::clicked, this, &Self::generate);
  connect(ui->widthInput, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &Self::generate);
  connect(ui->heightInput, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &Self::generate);

  connect(ui->seedInput, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &Self::generate);
  connect(ui->frequencyInput, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Self::generate);

  // init
  generate();
}

void MainWindow::on_GenerateButton_Clicked() { generate(); }

void MainWindow::on_NoiseCanvas_PreviewChanged() {
  ui->timeNoiseLabel->setText(QString::number(ui->noiseCanvasWidget->getTime4Create()));
  ui->meanNoiseLabel->setText(QString::number(ui->noiseCanvasWidget->getMean()));
  ui->maxNoiseLabel->setText(QString::number(ui->noiseCanvasWidget->getMax()));
  ui->minNoiseLabel->setText(QString::number(ui->noiseCanvasWidget->getMin()));
}
void MainWindow::generate() {
  NoiseCanvasWidget::PreviewConfig config;
  auto &noise = config.noise;
  noise.SetNoiseType(ui->noiseTypeComboBox->currentData().value<FastNoiseLite::NoiseType>());

  noise.SetSeed(ui->seedInput->value());
  float frequency = ui->frequencyInput->value();
  noise.SetFrequency(frequency);

  config.size = QSize(ui->widthInput->value(), ui->heightInput->value());
  config.dimension = ui->n3DCheckBox->checkState() == Qt::Checked ? decltype(config.dimension)::PREVIEW_DIMENSION_3D : decltype(config.dimension)::PREVIEW_DIMENSION_2D;
  config.dz = 1.f / frequency * 0.2;

  ui->noiseCanvasWidget->preview(config);
}
