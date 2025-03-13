# Calibration Map
A very simple utility to calculate correction values from a map of calibrated/known values.

# Usage
```c
CCalibrationMap CalibrationMap;

CalibrationMap.AddPoint(10.0, 9.8);
CalibrationMap.AddPoint(20.0, 19.5);

double CorrectedValue = CalibrationMap.CorrectedPoint(15.0);

 std::cout << "Corrected Position: " << CorrectedValue << std::endl;

```
