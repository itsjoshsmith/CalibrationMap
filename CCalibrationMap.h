#pragma once
#include <map>
#include <vector>
#include <stdexcept>
#include <sstream>

class CCalibrationMap
{
public:
  /// <summary>
  /// Adds a value set to the error map
  /// </summary>
  /// <param name="Nominal">The nominal value</param>
  /// <param name="Calibrated">The corresponding calibrated value</param>
  void AddPoint(double Nominal, double Calibrated) 
  {
    m_CalibratedMap[Nominal] = Nominal - Calibrated;
  }

  /// <summary>
  /// Adds a range of points to the calibrated map
  /// </summary>
  /// <param name="Nominals">Vector of nominal values</param>
  /// <param name="Calibrated">Vector of the corresponding calibrate values</param>
  /// <exception cref="std::invalid_argument">Thrown when the vector counrs are not equal. The nominals and calibrated vector sizes must be equal and sequentially corresponding</exception>
  void AddPoints(std::vector<double>& Nominals, std::vector<double>& Calibrated)
  {
    if (Nominals.size() != Calibrated.size())
      throw std::invalid_argument("Nominals and Calibrated vectors must have the same size.");
    
    for (size_t i = 0; i < Nominals.size(); ++i)
      AddPoint(Nominals[i], Calibrated[i]);
  }

  /// <summary>
  /// Sets the map passed in, to the internal calibration map
  /// </summary>
  /// <param name="Map"></param>
  void SetMap(std::map<double, double> Map)
  {
    m_CalibratedMap = Map;
  }

  /// <summary>
  /// Appends the internal calibration with the map passed in
  /// </summary>
  /// <param name="Map"></param>
  void AppendMap(std::map<double, double>& Map)
  {
    m_CalibratedMap.insert(Map.begin(), Map.end());
  }

  /// <summary>
  /// Returns the calculated error value for the position passed in
  /// </summary>
  /// <param name="Nominal">The nominal value </param>
  /// <returns>The error value for the nominal value passed in, taken from the error map</returns>
  /// <exception cref="std::runtime_error">Thrown when this function is called but the calibration map is empty</exception>
  /// <exception cref="std::out_of_range">Thrown when the nominal value is outside of the range of the calibration map</exception>
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

  /// <summary>
  /// Returns the corrected position for the nominal value passed in
  /// </summary>
  /// <param name="Nominal">The nominal value to be corrected against the error map</param>
  /// <returns>The corrected position taken from the error map</returns>
  /// <exception cref="std::runtime_error">Thrown when this function is called but the calibration map is empty</exception>
  /// <exception cref="std::out_of_range">Thrown when the nominal value is outside of the range of the calibration map</exception>
  double CorrectedPosition(double Nominal)
  {
    return Nominal - ErrorValue(Nominal);
  }

  /// <summary>
  /// Returns a summary of the calibration map
  /// </summary>
  /// <returns>A string in a string-tabbed format of the calibration map including errors and corrected values</returns>
  std::string GetMapSummary()
  {
    std::ostringstream summary;
    summary << "Nominal\tCalibrated\tError\tCorrected\n";
    for (auto it = m_CalibratedMap.begin(); it != m_CalibratedMap.end(); ++it) 
      summary << it->first << "\t" << it-> first - ErrorValue(it->first) << "\t\t" << ErrorValue(it->first) << "\t" << CorrectedPosition(it->first) << "\n";
    return summary.str();
  }

private:
  /// <summary>
  /// The map to hold the error values
  /// </summary>
  std::map<double, double> m_CalibratedMap;

  /// <summary>
  /// Interpolation helper function
  /// </summary>
  double Interpolate(double x, double x1, double y1, double x2, double y2)
  {
    return y1 + (x - x1) * (y2 - y1) / (x2 - x1);
  }
};

