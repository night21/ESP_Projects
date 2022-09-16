using System;
using System.Collections.Generic;
          
public class Program
{
  public static void Main()
  {
    List<TDto> data = new List<TDto>();
    data.Add(new TDto(92, 90, 89));
    data.Add(new TDto(92, 90.5f, 90.4f));
    data.Add(new TDto(92, 90.9f, 90.8f));
    data.Add(new TDto(92, 91f, 90.9f));
    data.Add(new TDto(92, 91.5f, 91f));
    data.Add(new TDto(92, 93.5f, 92.4f));
    data.Add(new TDto(92, 95.5f, 94.4f));
    data.Add(new TDto(92, 94.5f, 95.4f));
    data.Add(new TDto(92, 93.0f, 93.1f));
    data.Add(new TDto(92, 92.5f, 92.9f));
    data.Add(new TDto(92, 91.5f, 91.6f));
    
    foreach (TDto d in data)
    {
    Console.WriteLine(d.ToString() + " " + (IsBoiling(d.T, d.A, d.P) ? "boiling" : "Not boiling"));
    }
  
    
  }
  
  private static bool IsBoiling(float temperature, float actualTemperature, float prevTemperature)
  {
    float error = temperature - actualTemperature;
      bool increase = actualTemperature - prevTemperature > 0;
      return (increase && error > 1) || (!increase && error > -1);
    
  }
}

public class TDto
{
  public float T {get; private set;}
  public float A {get; private set;}
  public float P {get; private set;}
  
  public TDto(float t, float a, float p)
  {
    this.T = t;
    this.A = a;
    this.P = p;
  }
  
  public override string ToString()
  {
    return String.Format("Temp: {0}, Actual: {1}, Prev: {2}", T, A, P);
  }
  
}
