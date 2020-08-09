#ifndef GDAL_IO
#define GDAL_IO

// Standard Library
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// EXT
#include "cpl_conv.h"
#include "gdal_priv.h"
#include "glm.hpp"
#include <ogrsf_frmts.h>

namespace gdal_input {
/**
 * @brief The supported types of geometry for vector data.
 */
enum class GeometryType { UNKNOWN = 0, POINT, POLYLINE };
/**
 * @brief An abstraction of OGR Vector datasets to simplify reading vector data.
 */
class VectorDataset {
public:
  /**
   * @brief Construct a new Vector Dataset object
   *
   * @param filepath The filepath of the dataset to open. This path must point
   * to a vector dataset that GDAL has a driver implementation of (e.g.
   * shapefile, geopackage, geodatabase, etc...).
   */
  VectorDataset(std::string filepath);
  /**
   * @brief Destroy the Vector Dataset object
   */
  ~VectorDataset();
  /**
   * @brief Open a new vector dataset and close any previously opened dataset on
   * this object.
   *
   * @param filepath The filepath of the dataset to open. This path must point
   * to a vector dataset that GDAL has a driver implementation of (e.g.
   * shapefile, geopackage, geodatabase, etc...).
   */
  void open_dataset(std::string filepath);
  /**
   * @brief Read the field names of a layer in the dataset.
   *
   * @param layer_name The name of the layer to read field names from.
   * @return A vector of the field names in the layer.
   */
  std::vector<std::string> read_field_names(std::string layer_name);
  /**
   * @brief Read the layer names in the dataset.
   *
   * @return A vector of the layer names in the dataset.
   */
  std::vector<std::string> read_layer_names();
  /**
   * @brief Get the geometry type of a layer
   *
   * @param layer_name The name of the layer for which to query the geometry
   * type.
   * @return The GeometryType of the layer.
   */
  GeometryType get_layer_geometry_type(std::string layer_name);
  /**
   * @brief Get the number of features in a layer.
   *
   * @param layer_name The layer for which to query the number of features.
   * @return The number of features in the layer.
   */
  int64_t get_feature_count(std::string layer_name);
  /**
   * @brief Read the coordinates of a point in a layer of the dataset. The layer
   * geometry type must be GeometryType::POINT.
   *
   * @param layer_name The name of the layer to read the point from.
   * @param FID The feature ID of the point, 0 to
   * VectorDataset::get_feature_count(std::string).
   * @return The x,y,z coordinates of the point.
   */
  glm::dvec3 get_point_feature_geometry(std::string layer_name, int64_t FID);
  /**
   * @brief Read the coordinates of the set of points of a polyline feature in a
   * layer of the dataset. The layer geometry type must be
   * GeometryType::POLYLINE.
   *
   * @param layer_name The name of the layer to read the polylines from.
   * @param FID The feature ID of the polyline, 0 to
   * VectorDataset::get_feature_count(std::string).
   * @return A vector of the (x,y,z) points of the polyline.
   */
  std::vector<glm::dvec3> get_polyline_feature_geometry(std::string layer_name,
                                                        int64_t FID);
  /**
   * @brief Read an attribute of a feature in \p layer_name with \p field_name
   * as a double.
   *
   * @param layer_name The name of the layer to read from.
   * @param FID The feature ID of the feature to read from.
   * @param field_name The field name to read from.
   * @return The attribute of the feature interpreted as a double.
   */
  double get_field_as_double(std::string layer_name, int64_t FID,
                             std::string field_name);
  /**
   * @brief Read an attribute of a feature in \p layer_name with \p field_name
   * as a string.
   *
   * @param layer_name The name of the layer to read from.
   * @param FID The feature ID of the feature to read from.
   * @param field_name The field name to read from.
   * @return The attribute of the feature interpreted as a string.
   */
  std::string get_field_as_string(std::string layer_name, int64_t FID,
                                  std::string field_name);

private:
  GDALDataset *dataset = nullptr;
};
/**
 * @brief An abstraction of GDAL Raster datasets to simplfy reading raster data.
 */
class RasterDataset {
public:
  /**
   * @brief Construct a new Raster Dataset object
   *
   * @param filepath The filepath to the raster dataset. Must point to a
   * filetype that GDAL has a driver implementation for.
   */
  RasterDataset(std::string filepath);
  /**
   * @brief Destroy the Raster Dataset object
   */
  ~RasterDataset();
  /**
   * @brief Open a new raster dataset and close any previously opened raster
   * dataset on this object.
   *
   * @param filepath
   */
  void open_dataset(std::string filepath);
  /**
   * @brief Get the number of rows in the raster dataset.
   *
   * @return The number of rows in the raster dataset.
   */
  int get_rows();
  /**
   * @brief Get the number of columns in the raster dataset.
   *
   * @return The number of columns in the raster dataset.
   */
  int get_cols();
  /**
   * @brief Get the pixel scale of the raster dataset.
   * @details The scale of each pixel in worldspace. Units will be defined by
   * the spatial reference.
   *
   * @return A glm::dvec2 with the world space scale of each pixel in the x and
   * y direction.
   */
  glm::dvec2 get_pixel_scale();
  /**
   * @brief Get the top left coordinate in world space of the raster defined by
   * the spatial reference system.
   *
   * @return a glm::dvec2 of the top left coordinate in world space.
   */
  glm::dvec2 get_top_left_coord();
  /**
   * @brief Reads floats from a raster band. If access is out of bounds, junk
   * data is returned.
   *
   * @param band The band index to read from starting a 1.
   * @param x0 The first column of the raster band to read from.
   * @param xf The last column of the raster band to read from.
   * @param y0 The first row of the raster band to read from.
   * @param yf The last row of the raster band to read from.
   * @param n The number of columns to report. Must be <= xf-x0+1.
   * @param m The number of rows to report. Must be <= yf-y0+1.
   * @return The floats read from the band. Left to right and up to down.
   * Resolution may be smaller than the raw raster size depending on choice of n
   * and m.
   */
  std::vector<float> read_floats(int band, int x0, int xf, int y0, int yf,
                                 int n, int m, float offset = 0.0f);
  /**
   * @brief Reads bytes from a raster band. If access is out of bounds, junk
   * data is returned.
   *
   * @param band The band index to read from starting a 1.
   * @param x0 The first column of the raster band to read from.
   * @param xf The last column of the raster band to read from.
   * @param y0 The first row of the raster band to read from.
   * @param yf The last row of the raster band to read from.
   * @param n The number of columns to report. Must be <= xf-x0+1.
   * @param m The number of rows to report. Must be <= yf-y0+1.
   * @return The bytes read from the band. Left to right and up to down.
   * Resolution may be smaller than the raw raster size depending on choice of n
   * and m.
   */
  std::vector<unsigned char> read_bytes(int band, int x0, int xf, int y0,
                                        int yf, int n, int m);
  float get_no_data_float(int band);

private:
  GDALDataset *dataset = nullptr;
};
/**
 * @brief Open the operating system's open file dialog box and return the
 * absolute path to the selected file.
 *
 * @param extension (Optional) A list of file extensions to filter by. This is a
 * comma separated string of the extension names with no "." prefixed.
 * @return The absolute path to the selected file.
 */
std::string open_file_dialog(const char *extension = nullptr);
} // namespace gdal_input
#endif
