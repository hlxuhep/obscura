#ifndef __Direct_Detection_Crystal_hpp_
#define __Direct_Detection_Crystal_hpp_

#include <string>
#include <vector>

#include "obscura/DM_Distribution.hpp"
#include "obscura/DM_Particle.hpp"
#include "obscura/Direct_Detection.hpp"
#include "obscura/Target_Crystal.hpp"

namespace obscura
{

// 1. Event spectra and rates
extern double dRdEe_Crystal(double Ee, const DM_Particle& DM, DM_Distribution& DM_distr, Crystal& target_crystal);
extern double R_Q_Crystal(int Q, const DM_Particle& DM, DM_Distribution& DM_distr, Crystal& target_crystal);
extern double R_total_Crystal(int Qthreshold, const DM_Particle& DM, DM_Distribution& DM_distr, Crystal& target_crystal);

// 2. Electron recoil direct detection experiment with semiconductor target
class DM_Detector_Crystal : public DM_Detector
{
  private:
	Crystal target_crystal;

	// Q spectrum
	unsigned int Q_threshold;

	// (a) Poisson: Q threshold
	bool using_Q_threshold;
    std::vector<double> DM_Signals_Q_Threshold(const DM_Particle& DM, DM_Distribution& DM_distr);
	
	// (b) Binned Poisson: Q bins
	bool using_Q_bins;
	std::vector<double> DM_Signals_Q_Bins(const DM_Particle& DM, DM_Distribution& DM_distr);

  public:
	DM_Detector_Crystal();
	DM_Detector_Crystal(std::string label, double expo, std::string crys);

	// DM functions
	virtual double Maximum_Energy_Deposit(DM_Particle& DM, const DM_Distribution& DM_distr) const override;
	virtual double Minimum_DM_Speed(DM_Particle& DM) const override;
	virtual double Minimum_DM_Mass(DM_Particle& DM, const DM_Distribution& DM_distr) const override;
	virtual double dRdE(double E, const DM_Particle& DM, DM_Distribution& DM_distr) override;
	virtual double DM_Signals_Total(const DM_Particle& DM, DM_Distribution& DM_distr) override;
	virtual std::vector<double> DM_Signals_Binned(const DM_Particle& DM, DM_Distribution& DM_distr) override;

	// Q spectrum
	//  (a) Poisson
	void Use_Q_Threshold(unsigned int Q_thr, unsigned int N_bins = 0);
	// (b) Binned Poisson
	void Use_Q_Bins(unsigned int Q_thr, unsigned int N_bins = 0);

	virtual void Print_Summary(int MPI_rank = 0) const override;
};

}	// namespace obscura

#endif