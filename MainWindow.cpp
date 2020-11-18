//
// Created by chudonghao on 2020/11/17.
//

#include "MainWindow.h"

#include <QDesktopServices>
#include <QFileDialog>

#include <FastNoiseLite.h>

#include "ui_MainWindow.h"

Q_DECLARE_METATYPE(FastNoiseLite::NoiseType)
Q_DECLARE_METATYPE(FastNoiseLite::RotationType3D)
Q_DECLARE_METATYPE(FastNoiseLite::FractalType)
Q_DECLARE_METATYPE(FastNoiseLite::CellularDistanceFunction)
Q_DECLARE_METATYPE(FastNoiseLite::CellularReturnType)
Q_DECLARE_METATYPE(FastNoiseLite::DomainWarpType)

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

  // fractal type
  std::map<QString, FastNoiseLite::FractalType> name2FractalTypeMap = {
    {"None", FastNoiseLite::FractalType::FractalType_None},
    {"FBm", FastNoiseLite::FractalType::FractalType_FBm},
    {"Ridged", FastNoiseLite::FractalType::FractalType_Ridged},
    {"PingPong", FastNoiseLite::FractalType::FractalType_PingPong},
    {"DomainWarpIndependent", FastNoiseLite::FractalType::FractalType_DomainWarpIndependent},
    {"DomainWarpProgressive", FastNoiseLite::FractalType::FractalType_DomainWarpProgressive},
  };
  for (auto &p : name2FractalTypeMap) {
    ui->fractalTypeComboBox->addItem(p.first, p.second);
  }
  ui->fractalTypeComboBox->setCurrentText("None");

  // cellular distance function
  std::map<QString, FastNoiseLite::CellularDistanceFunction> name2CellularDistanceFunctionMap = {
    {"Euclidean", FastNoiseLite::CellularDistanceFunction::CellularDistanceFunction_Euclidean},
    {"EuclideanSq", FastNoiseLite::CellularDistanceFunction::CellularDistanceFunction_EuclideanSq},
    {"Hybrid", FastNoiseLite::CellularDistanceFunction::CellularDistanceFunction_Hybrid},
    {"Manhattan", FastNoiseLite::CellularDistanceFunction::CellularDistanceFunction_Manhattan},
  };
  for (auto &p : name2CellularDistanceFunctionMap) {
    ui->cellularDistanceFunctionComboBox->addItem(p.first, p.second);
  }
  ui->cellularDistanceFunctionComboBox->setCurrentText("EuclideanSq");

  // cellular return type
  std::map<QString, FastNoiseLite::CellularReturnType> name2CellularReturnTypeMap = {
    {"CellValue", FastNoiseLite::CellularReturnType::CellularReturnType_CellValue},       {"Distance", FastNoiseLite::CellularReturnType::CellularReturnType_Distance},
    {"Distance2", FastNoiseLite::CellularReturnType::CellularReturnType_Distance2},       {"Distance2Add", FastNoiseLite::CellularReturnType::CellularReturnType_Distance2Add},
    {"Distance2Sub", FastNoiseLite::CellularReturnType::CellularReturnType_Distance2Sub}, {"Distance2Mul", FastNoiseLite::CellularReturnType::CellularReturnType_Distance2Mul},
    {"Distance2Div", FastNoiseLite::CellularReturnType::CellularReturnType_Distance2Div},
  };
  for (auto &p : name2CellularReturnTypeMap) {
    ui->cellularReturnTypeComboBox->addItem(p.first, p.second);
  }
  ui->cellularReturnTypeComboBox->setCurrentText("Distance");

  // domain warp type
  std::map<QString, FastNoiseLite::DomainWarpType> name2DomainWarpTypeMap = {
    {"None", static_cast<FastNoiseLite::DomainWarpType>(-1)},
    {"BasicGrid", FastNoiseLite::DomainWarpType::DomainWarpType_BasicGrid},
    {"OpenSimplex2", FastNoiseLite::DomainWarpType::DomainWarpType_OpenSimplex2},
    {"OpenSimplex2Reduced", FastNoiseLite::DomainWarpType::DomainWarpType_OpenSimplex2Reduced},
  };
  for (auto &p : name2DomainWarpTypeMap) {
    ui->domainWarpTypeComboBox->addItem(p.first, p.second);
  }
  ui->domainWarpTypeComboBox->setCurrentText("None");

  // signals
  connect(ui->noiseCanvasWidget, &NoiseCanvasWidget::previewChanged, this, &Self::on_NoiseCanvas_PreviewChanged);

  connect(ui->generateButton, &QPushButton::clicked, this, &Self::callPreview);
  connect(ui->scrollUpButton, &QPushButton::clicked, ui->noiseCanvasWidget, [=]() { ui->noiseCanvasWidget->scrollUp(); });
  connect(ui->scrollDownButton, &QPushButton::clicked, ui->noiseCanvasWidget, [=]() { ui->noiseCanvasWidget->scrollUp(); });
  connect(ui->scrollButton, &QPushButton::clicked, ui->noiseCanvasWidget, &NoiseCanvasWidget::setScroll);

  connect(ui->widthInput, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &Self::callPreview);
  connect(ui->heightInput, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &Self::callPreview);
  connect(ui->invertCheckBox, &QCheckBox::stateChanged, this, &Self::callPreview);
  connect(ui->n3DCheckBox, &QCheckBox::stateChanged, this, &Self::callPreview);
  connect(ui->visualiseDomainWarpCheckBox, &QCheckBox::stateChanged, this, &Self::callPreview);

  connect(ui->noiseTypeComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &Self::callPreview);
  connect(ui->rotationType3DComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &Self::callPreview);
  connect(ui->seedInput, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &Self::callPreview);
  connect(ui->frequencyInput, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Self::callPreview);

  connect(ui->fractalTypeComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &Self::callPreview);
  connect(ui->fractalOctavesInput, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &Self::callPreview);
  connect(ui->fractalLacunarityInput, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Self::callPreview);
  connect(ui->fractalGainInput, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Self::callPreview);
  connect(ui->fractalWeightedStrengthInput, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Self::callPreview);
  connect(ui->fractalPingPongStrengthInput, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Self::callPreview);

  connect(ui->cellularDistanceFunctionComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &Self::callPreview);
  connect(ui->cellularReturnTypeComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &Self::callPreview);
  connect(ui->cellularJitterInput, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Self::callPreview);

  connect(ui->domainWarpTypeComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &Self::callPreview);
  connect(ui->domainWarpAmpInput, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &Self::callPreview);

  connect(ui->saveButton, &QPushButton::clicked, this, &Self::on_SaveButton_Clicked);
  connect(ui->githubButton, &QPushButton::clicked, this, &Self::on_GithubButton_Clicked);
  // init
  callPreview();
}

void MainWindow::on_NoiseCanvas_PreviewChanged() {
  ui->timeNoiseLabel->setText(QString::number(ui->noiseCanvasWidget->getTime4Create()));
  ui->meanNoiseLabel->setText(QString::number(ui->noiseCanvasWidget->getMean()));
  ui->maxNoiseLabel->setText(QString::number(ui->noiseCanvasWidget->getMax()));
  ui->minNoiseLabel->setText(QString::number(ui->noiseCanvasWidget->getMin()));
}

void MainWindow::on_SaveButton_Clicked() {
  QString fileName = QFileDialog::getSaveFileName(nullptr, "Save Noise Image", ".", "Image (*.png *.jpg)");
  if (fileName.isEmpty()) {
    return;
  }
  const QImage &img = ui->noiseCanvasWidget->getCurrentImage();
  img.save(fileName);
  statusBar()->showMessage(QString("Save to file %1.").arg(fileName), 3000);
}

void MainWindow::on_GithubButton_Clicked() { QDesktopServices::openUrl(QUrl("https://github.com/chudonghao/FastNoisePreviewAppQt")); }

void MainWindow::callPreview() {
  NoiseCanvasWidget::PreviewConfig config;
  auto &noise = config.noise;

  noise.SetRotationType3D(ui->rotationType3DComboBox->currentData().value<FastNoiseLite::RotationType3D>());
  noise.SetNoiseType(ui->noiseTypeComboBox->currentData().value<FastNoiseLite::NoiseType>());
  noise.SetSeed(ui->seedInput->value());
  noise.SetFrequency(ui->frequencyInput->value());

  noise.SetFractalType(ui->fractalTypeComboBox->currentData().value<FastNoiseLite::FractalType>());
  noise.SetFractalOctaves(ui->fractalOctavesInput->value());
  noise.SetFractalLacunarity(ui->fractalLacunarityInput->value());
  noise.SetFractalGain(ui->fractalGainInput->value());
  noise.SetFractalWeightedStrength(ui->fractalWeightedStrengthInput->value());
  noise.SetFractalPingPongStrength(ui->fractalPingPongStrengthInput->value());

  noise.SetCellularDistanceFunction(ui->cellularDistanceFunctionComboBox->currentData().value<FastNoiseLite::CellularDistanceFunction>());
  noise.SetCellularReturnType(ui->cellularReturnTypeComboBox->currentData().value<FastNoiseLite::CellularReturnType>());
  noise.SetCellularJitter(ui->cellularJitterInput->value());

  noise.SetDomainWarpType(ui->domainWarpTypeComboBox->currentData().value<FastNoiseLite::DomainWarpType>());
  noise.SetDomainWarpAmp(ui->domainWarpAmpInput->value());

  config.size = QSize(ui->widthInput->value(), ui->heightInput->value());
  config.invert = ui->invertCheckBox->checkState() == Qt::Checked;
  config.dimension = ui->n3DCheckBox->checkState() == Qt::Checked ? decltype(config.dimension)::PREVIEW_DIMENSION_3D : decltype(config.dimension)::PREVIEW_DIMENSION_2D;
  config.visualiseDomainWarp = ui->visualiseDomainWarpCheckBox->checkState() == Qt::Checked;

  ui->noiseCanvasWidget->preview(config);
}
