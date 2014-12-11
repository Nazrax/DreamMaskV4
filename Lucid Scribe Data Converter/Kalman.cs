using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Lucid_Scribe_Data_Converter
{
  public class Kalman
  {
    private double Q = 0.000001;
    private double R = 0.01;
    private double P = 1, X = 0, K;

    public int Update(int measurement)
    {
      K = (P + Q) / (P + Q + R);
      P = R * (P + Q) / (R + P + Q);
      X = X + (measurement - X) * K;
      return (int)X;
    }
  }
}
