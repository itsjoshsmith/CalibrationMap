/**
 * @file CCalibrationMap.h
 * @brief Defines the CCalibrationMap class for managing calibration data.
 *
 * This class provides a mapping between nominal and calibrated values, allowing
 * for error correction and interpolation. It supports adding single or multiple
 * calibration points, retrieving error values, and computing corrected positions.
 *
 * @author Josh Smith
 * @date 3/12/2024
 * @version V1.0
 *
 * @details
 * Features:
 * - Store and retrieve calibration data efficiently.
 * - Interpolate missing values using linear interpolation.
 * - Compute corrected positions based on calibration errors.
 * - Provide a formatted summary of the calibration map.
 *
 * Usage Example:
 * @code
 * CCalibrationMap calibMap;
 * calibMap.AddPoint(10.0, 9.8);
 * calibMap.AddPoint(20.0, 19.5);
 * double corrected = calibMap.CorrectedPosition(15.0);
 * std::cout << "Corrected Position: " << corrected << std::endl;
 * @endcode
 *
 */

#pragma once
#include <map>
#include <vector>
#include <stdexcept>
#include <sstream>

 /**
  * @class CCalibrationMap
  * @brief Manages a calibration map for correcting nominal values.
  *
  * This class stores calibration data, allowing for error correction
  * and interpolation. It supports adding data points, retrieving errors,
  * and computing corrected positions.
  */
class CCalibrationMap
{
public:
  /**
   * @brief Adds a single calibration point.
   * @param Nominal The nominal value.
   * @param Calibrated The corresponding calibrated value.
   */
  void AddPoint(double Nominal, double Calibrated)
  {
    m_CalibratedMap[Nominal] = Nominal - Calibrated;
  }

  /**
   * @brief Adds multiple calibration points.
   * @param Nominals Vector of nominal values.
   * @param Calibrated Vector of corresponding calibrated values.
   * @throws std::invalid_argument if the vector sizes do not match.
   */
  void AddPoints(std::vector<double>& Nominals, std::vector<double>& Calibrated)
  {
    if (Nominals.size() != Calibrated.size())
      throw std::invalid_argument("Nominals and Calibrated vectors must have the same size.");

    for (size_t i = 0; i < Nominals.size(); ++i)
      AddPoint(Nominals[i], Calibrated[i]);
  }

  /**
   * @brief Sets the calibration map.
   * @param Map A map of nominal values and their error values.
   */
  void SetMap(std::map<double, double> Map)
  {
    m_CalibratedMap = Map;
  }

  /**
   * @brief Appends additional calibration data to the existing map.
   * @param Map A map of nominal values and their error values.
   */
  void AppendMap(std::map<double, double>& Map)
  {
    m_CalibratedMap.insert(Map.begin(), Map.end());
  }

  /**
   * @brief Retrieves the error value for a given nominal input.
   * @param Nominal The nominal value.
   * @return The error value from the calibration map.
   * @throws std::runtime_error if the map is empty.
   * @throws std::out_of_range if the nominal value is outside the map range.
   */
  double ErrorValue(double Nominal)
  {
    if (m_CalibratedMap.empty())
      throw std::runtime_error("Calibration map is empty.");

    auto it = m_CalibratedMap.find(Nominal);
    if (it != m_CalibratedMap.end())
      return it->second;

    auto upper = m_CalibratedMap.upper_bound(Nominal);
    auto lower = (upper == m_CalibratedMap.begin()) ? m_CalibratedMap.end() : std::prev(upper);

    if (lower == m_CalibratedMap.end() || upper == m_CalibratedMap.end())
      throw std::out_of_range("Nominal value outside of calibrated range.");

    return Interpolate(Nominal, lower->first, lower->second, upper->first, upper->second);
  }

  /**
   * @brief Computes the corrected position for a nominal value.
   * @param Nominal The nominal value to be corrected.
   * @return The corrected position.
   * @throws std::runtime_error if the map is empty.
   * @throws std::out_of_range if the nominal value is outside the map range.
   */
  double CorrectedPosition(double Nominal)
  {
    return Nominal - ErrorValue(Nominal);
  }

  /**
   * @brief Returns a summary of the calibration map.
   * @return A formatted string containing the nominal, calibrated, error, and corrected values.
   */
  std::string GetMapSummary()
  {
    std::ostringstream summary;
    summary << "Nominal\tCalibrated\tError\tCorrected\n";
    for (auto it = m_CalibratedMap.begin(); it != m_CalibratedMap.end(); ++it)
      summary << it->first << "\t" << it->first - ErrorValue(it->first) << "\t\t"
      << ErrorValue(it->first) << "\t" << CorrectedPosition(it->first) << "\n";
    return summary.str();
  }

private:
  /**
   * @brief Holds the calibration error values.
   */
  std::map<double, double> m_CalibratedMap;

  /**
   * @brief Performs linear interpolation between two points.
   * @param x The x-value to interpolate.
   * @param x1 The first known x-value.
   * @param y1 The first known y-value.
   * @param x2 The second known x-value.
   * @param y2 The second known y-value.
   * @return The interpolated y-value.
   */
  double Interpolate(double x, double x1, double y1, double x2, double y2)
  {
    return y1 + (x - x1) * (y2 - y1) / (x2 - x1);
  }
};

