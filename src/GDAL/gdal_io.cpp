#include "GDAL/gdal_io.hpp"
// Standard Library
#include <algorithm>
#include <iostream>
#include <string>
#include <thread>

// EXT
#include "nfd.h"

namespace gdal_input {
VectorDataset::VectorDataset(std::string filepath) { open_dataset(filepath); }
VectorDataset::~VectorDataset() { GDALClose(dataset); }
void VectorDataset::open_dataset(std::string filepath) {
  if (dataset) {
    GDALClose(dataset);
  }
  dataset = static_cast<GDALDataset *>(
      GDALOpenEx(filepath.c_str(), GDAL_OF_VECTOR, NULL, NULL, NULL));
  if (dataset == nullptr) {
    std::cerr << "Error: Could not open vector dataset: " << filepath
              << std::endl;
  }
}
std::vector<std::string>
VectorDataset::read_field_names(std::string layer_name) {
  std::vector<std::string> field_names{};
  if (dataset) {
    OGRLayer *layer;
    layer = dataset->GetLayerByName(layer_name.c_str());
    if (layer) {
      layer->ResetReading();
      for (int i = 0; i < layer->GetLayerDefn()->GetFieldCount(); i++) {
        field_names.push_back(
            layer->GetLayerDefn()->GetFieldDefn(i)->GetNameRef());
      }
      return field_names;
    }
  }
  return field_names;
}
std::vector<std::string> VectorDataset::read_layer_names() {
  std::vector<std::string> layer_names{};
  if (dataset) {
    for (int i = 0; i < dataset->GetLayerCount(); i++) {
      layer_names.push_back(dataset->GetLayer(i)->GetName());
    }
  }
  return layer_names;
}
GeometryType VectorDataset::get_layer_geometry_type(std::string layer_name) {
  GeometryType geo_type = GeometryType::UNKNOWN;
  if (dataset) {
    OGRLayer *layer;
    layer = dataset->GetLayerByName(layer_name.c_str());
    if (layer) {
      OGRwkbGeometryType wkb_type = wkbFlatten(layer->GetGeomType());
      switch (wkb_type) {
      case wkbPoint:
        geo_type = GeometryType::POINT;
        break;
      case wkbLineString:
        geo_type = GeometryType::POLYLINE;
        break;
      default:
        geo_type = GeometryType::UNKNOWN;
        break;
      }
    }
  }
  return geo_type;
}
int64_t VectorDataset::get_feature_count(std::string layer_name) {
  if (dataset) {
    return dataset->GetLayerByName(layer_name.c_str())->GetFeatureCount();
  }
  return 0;
}
glm::dvec3 VectorDataset::get_point_feature_geometry(std::string layer_name,
                                                     int64_t FID) {
  glm::dvec3 result{};
  if (get_layer_geometry_type(layer_name) == GeometryType::POINT) {
    OGRLayer *layer = dataset->GetLayerByName(layer_name.c_str());
    if (layer && layer->GetFeatureCount() > FID) {
      OGRFeature *feature = layer->GetFeature(FID);
      if (feature != nullptr) {
        OGRPoint *point = feature->GetGeometryRef()->toPoint();
        result.x = point->getX();
        result.y = point->getY();
        result.z = point->getZ();
      }
      OGRFeature::DestroyFeature(feature);
    }
  }
  return result;
}
std::vector<glm::dvec3>
VectorDataset::get_polyline_feature_geometry(std::string layer_name,
                                             int64_t FID) {
  std::vector<glm::dvec3> result{};
  if (get_layer_geometry_type(layer_name) == GeometryType::POLYLINE) {
    OGRLayer *layer = dataset->GetLayerByName(layer_name.c_str());
    if (layer && layer->GetFeatureCount() > FID) {
      OGRFeature *feature = layer->GetFeature(FID);
      if (feature != nullptr) {
        OGRLineString *polyline = feature->GetGeometryRef()->toLineString();
        OGRPointIterator *point_iter = polyline->getPointIterator();
        OGRPoint ogr_point;
        while (point_iter->getNextPoint(&ogr_point)) {
          result.push_back(
              {ogr_point.getX(), ogr_point.getY(), ogr_point.getZ()});
        }
        OGRPointIterator::destroy(point_iter);
      }
      OGRFeature::DestroyFeature(feature);
    }
  }
  return result;
}
double VectorDataset::get_field_as_double(std::string layer_name, int64_t FID,
                                          std::string field_name) {
  double result{};
  OGRLayer *layer = dataset->GetLayerByName(layer_name.c_str());
  if (layer && layer->GetFeatureCount() > FID) {
    OGRFeature *feature = layer->GetFeature(FID);
    if (feature != nullptr) {
      result = feature->GetFieldAsDouble(field_name.c_str());
    }
    OGRFeature::DestroyFeature(feature);
  }
  return result;
}
std::string VectorDataset::get_field_as_string(std::string layer_name,
                                               int64_t FID,
                                               std::string field_name) {
  std::string result{};
  OGRLayer *layer = dataset->GetLayerByName(layer_name.c_str());
  if (layer && layer->GetFeatureCount() > FID) {
    OGRFeature *feature = layer->GetFeature(FID);
    if (feature != nullptr) {
      result = feature->GetFieldAsString(field_name.c_str());
    }
    OGRFeature::DestroyFeature(feature);
  }
  return result;
}

RasterDataset::RasterDataset(std::string filepath) { open_dataset(filepath); }
RasterDataset::~RasterDataset() { GDALClose(dataset); }
void RasterDataset::open_dataset(std::string filepath) {
  if (dataset) {
    GDALClose(dataset);
  }
  dataset = static_cast<GDALDataset *>(GDALOpen(filepath.c_str(), GA_ReadOnly));
  if (dataset == nullptr) {
    std::cerr << "Error: Could not open raster dataset: " << filepath
              << std::endl;
  }
}
int RasterDataset::get_rows() {
  if (dataset) {
    return dataset->GetRasterYSize();
  }
  return 0;
}
int RasterDataset::get_cols() {
  if (dataset) {
    return dataset->GetRasterXSize();
  }
  return 0;
}
glm::dvec2 RasterDataset::get_pixel_scale() {
  glm::dvec2 scale{};
  double geoTransform[6];
  if (dataset->GetGeoTransform(geoTransform) == CE_None) {
    scale = {geoTransform[1], -geoTransform[5]};
  }
  return scale;
}
glm::dvec2 RasterDataset::get_top_left_coord() {
  glm::dvec2 top_left{};
  double geoTransform[6];
  if (dataset->GetGeoTransform(geoTransform) == CE_None) {
    top_left = {geoTransform[0], geoTransform[3]};
  }
  return top_left;
}
std::vector<float> RasterDataset::read_floats(int band, int x0, int xf, int y0,
                                              int yf, int n, int m,
                                              float offset) {
  // calculate out of bounds offset
  int u = -std::min(x0, 0);
  int v = -std::min(y0, 0);
  // calc resolution scale
  int sx = (xf - x0 + 1) / n;
  int sy = (yf - y0 + 1) / m;
  // clamp row and column to raster bounds
  x0 = glm::clamp(x0, 0, get_cols() - 1);
  xf = glm::clamp(xf, 0, get_cols() - 1);
  y0 = glm::clamp(y0, 0, get_rows() - 1);
  yf = glm::clamp(yf, 0, get_rows() - 1);
  int cols = xf - x0 + 1;
  int rows = yf - y0 + 1;
  int read_n = cols / sx;
  int read_m = rows / sy;
  GDALRasterBand *raster_band = dataset->GetRasterBand(band);
  float no_data_value = raster_band->GetNoDataValue();
  float *data;
  data = (float *)CPLMalloc(sizeof(float) * read_n * read_m);
  auto err = raster_band->RasterIO(GF_Read, x0, y0, cols, rows, data, read_n,
                                   read_m, GDT_Float32, 0, 0);
  std::vector<float> data_vec{};
  int k = 0;
  for (int j = 0; j < m; j++) {
    for (int i = 0; i < n; i++) {
      if ((i >= u) && (i < read_n + u) && (j >= v) && (j < read_m + v)) {
        data_vec.push_back(data[k] - offset);
        k++;
      } else {
        data_vec.push_back(no_data_value);
      }
    }
  }
  CPLFree(data);
  return data_vec;
}
std::vector<unsigned char> RasterDataset::read_bytes(int band, int x0, int xf,
                                                     int y0, int yf, int n,
                                                     int m) {
  // calculate out of bounds offset
  int u = -std::min(x0, 0);
  int v = -std::min(y0, 0);
  // calc resolution scale
  int sx = (xf - x0 + 1) / n;
  int sy = (yf - y0 + 1) / m;
  // clamp row and column to raster bounds
  x0 = glm::clamp(x0, 0, get_cols() - 1);
  xf = glm::clamp(xf, 0, get_cols() - 1);
  y0 = glm::clamp(y0, 0, get_rows() - 1);
  yf = glm::clamp(yf, 0, get_rows() - 1);
  int cols = xf - x0 + 1;
  int rows = yf - y0 + 1;
  int read_n = cols / sx;
  int read_m = rows / sy;
  GDALRasterBand *raster_band = dataset->GetRasterBand(band);
  unsigned char no_data_value = raster_band->GetNoDataValue();
  unsigned char *data;
  data = (unsigned char *)CPLMalloc(sizeof(unsigned char) * read_n * read_m);
  auto err = raster_band->RasterIO(GF_Read, x0, y0, cols, rows, data, read_n,
                                   read_m, GDT_Byte, 0, 0);
  std::vector<unsigned char> data_vec{};
  int k = 0;
  for (int j = 0; j < m; j++) {
    for (int i = 0; i < n; i++) {
      if ((i >= u) && (i < read_n + u) && (j >= v) && (j < read_m + v)) {
        data_vec.push_back(data[k]);
        k++;
      } else {
        data_vec.push_back(no_data_value);
      }
    }
  }
  CPLFree(data);
  return data_vec;
}

float RasterDataset::get_no_data_float(int band) {
  GDALRasterBand *raster_band = dataset->GetRasterBand(band);
  return raster_band->GetNoDataValue();
}

std::string open_file_dialog(const char *extension) {
  nfdchar_t *filepath = NULL;
  nfdresult_t result = NFD_OpenDialog(extension, NULL, &filepath);
  if (result == NFD_OKAY) {
    // Successful read
    return std::string(filepath);
  } else if (result == NFD_CANCEL) {
    // User pressed cancel
    return "";
  } else {
    // Error, run NFD_GetError()
    std::cerr << "Error: Open File Dialog failed!" << std::endl;
    return "";
  }
}
} // namespace gdal_input