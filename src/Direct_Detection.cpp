#include "obscura/Direct_Detection.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>

#include "libphysica/Integration.hpp"
#include "libphysica/Natural_Units.hpp"
#include "libphysica/Special_Functions.hpp"
#include "libphysica/Statistics.hpp"
#include "libphysica/Utilities.hpp"

namespace obscura
{
using namespace libphysica::natural_units;

// DM Detector base class, which provides the statistical methods and energy bins.
// Statistics
// Likelihoods
double DM_Detector::Log_Likelihood(DM_Particle& DM, DM_Distribution& DM_distr)
{
	if(statistical_analysis == "Poisson")
	{
		double s			= DM_Signals_Total(DM, DM_distr);
		unsigned long int n = observed_events;
		double b			= expected_background;
		if(b < 1.0e-4 && (n > s)) b = n-s;	// see eq.(29) of [arXiv:1705.07920]
		return libphysica::Log_Likelihood_Poisson(s, n, b);
	}
	else if(statistical_analysis == "Binned Poisson")
	{
		std::vector<double> s			 = DM_Signals_Binned(DM, DM_distr);
		std::vector<unsigned long int> n = bin_observed_events;
		std::vector<double> b			 = bin_expected_background;
		for(unsigned int i = 0; i < b.size(); i++)
		if(b[i] < 1.0e-4 && (n[i] > s[i])) b[i] = n[i]-s[i]; // see eq.(29) of [arXiv:1705.07920]
		return libphysica::Log_Likelihood_Poisson_Binned(s, n, b);
	}
	else if(statistical_analysis == "Maximum Gap")
	{
		return log(P_Value_Maximum_Gap(DM, DM_distr));
	}
	else
	{
		std::cerr << libphysica::Formatted_String("Error", "Red", true) << " in obscura::DM_Detector_Nucleus::Log_Likelihood(): Analysis " << statistical_analysis << " not recognized." << std::endl;
		std::exit(EXIT_FAILURE);
	}
}

double DM_Detector::Likelihood(DM_Particle& DM, DM_Distribution& DM_distr)
{
	return exp(Log_Likelihood(DM, DM_distr));
}

std::vector<std::vector<double>> DM_Detector::Log_Likelihood_Scan(DM_Particle& DM, DM_Distribution& DM_distr, const std::vector<double>& masses, const std::vector<double>& couplings)
{
	double m_original		 = DM.mass;
	double coupling_original = DM.Get_Interaction_Parameter(targets);
	std::vector<std::vector<double>> log_likelihoods;
	for(auto& mass : masses)
	{
		DM.Set_Mass(mass);
		for(auto& coupling : couplings)
		{
			DM.Set_Interaction_Parameter(coupling, targets);
			log_likelihoods.push_back({mass, coupling, Log_Likelihood(DM, DM_distr)});
		}
	}
	DM.Set_Mass(m_original);
	DM.Set_Interaction_Parameter(coupling_original, targets);
	return log_likelihoods;
}

double DM_Detector::P_Value(DM_Particle& DM, DM_Distribution& DM_distr)
{
	double p_value = 1.0;
	if(statistical_analysis == "Poisson")
	{
		double DM_expectation_value;
		if(using_fiducial_values)
		{
			double coupling = DM.Get_Interaction_Parameter(targets);

			int rescaling_power = 2;
			if(DM.Interaction_Parameter_Is_Cross_Section())
				rescaling_power = 1;

			DM_expectation_value = pow(coupling / fiducial_coupling, rescaling_power) * fiducial_signals;
		}
		else
			DM_expectation_value = DM_Signals_Total(DM, DM_distr);
		p_value = libphysica::CDF_Poisson(DM_expectation_value + expected_background, observed_events);
	}
	else if(statistical_analysis == "Binned Poisson")
	{
		std::vector<double> expectation_values;
		if(using_fiducial_values)
		{
			double coupling		= DM.Get_Interaction_Parameter(targets);
			int rescaling_power = 2;
			if(DM.Interaction_Parameter_Is_Cross_Section())
				rescaling_power = 1;
			for(unsigned int i = 0; i < fiducial_spectrum.size(); i++)
				expectation_values.push_back(pow(coupling / fiducial_coupling, rescaling_power) * fiducial_spectrum[i]);
		}
		else
			expectation_values = DM_Signals_Binned(DM, DM_distr);
		std::vector<double> p_values(number_of_bins, 0.0);
		for(unsigned int i = 0; i < number_of_bins; i++)
		{
			double expectation_value = expectation_values[i] + bin_expected_background[i];
			p_values[i]				 = libphysica::CDF_Poisson(expectation_value, bin_observed_events[i]);
		}
		p_value = *std::min_element(p_values.begin(), p_values.end());
	}
	else if(statistical_analysis == "Maximum Gap")
	{
		p_value = P_Value_Maximum_Gap(DM, DM_distr);
	}
	else
	{
		std::cerr << libphysica::Formatted_String("Error", "Red", true) << " in obscura::DM_Detector_Nucleus::P_Value(): Analysis " << statistical_analysis << " not recognized." << std::endl;
		std::exit(EXIT_FAILURE);
	}

	return p_value;
}

// (a) Poisson statistics
void DM_Detector::Initialize_Poisson()
{
	statistical_analysis = "Poisson";
	observed_events		 = 0;
	expected_background	 = 0;
}

void DM_Detector::Set_Observed_Events(unsigned long int n)
{
	if(statistical_analysis != "Poisson")
	{
		std::cerr << libphysica::Formatted_String("Error", "Red", true) << " in obscura::DM_Detector::Set_Observed_Events(unsigned long int): Statistical analysis is " << statistical_analysis << " not 'Poisson'." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	else
	{
		observed_events = n;
	}
}

void DM_Detector::Set_Expected_Background(double B)
{
	if(statistical_analysis != "Poisson")
	{
		std::cerr << libphysica::Formatted_String("Error", "Red", true) << " in obscura::DM_Detector::Set_Expected_Background(double): Statistical analysis is " << statistical_analysis << " not 'Poisson'." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	else
	{
		expected_background = B;
	}
}

// (b) Binned Poisson statistics
void DM_Detector::Initialize_Binned_Poisson(unsigned bins)
{
	if(statistical_analysis == "Binned Poisson")
	{
		std::cerr << libphysica::Formatted_String("Error", "Red", true) << " in obscura::DM_Detector::Initialize_Binned_Poisson(): Bins have already been defined." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	else
	{
		statistical_analysis	= "Binned Poisson";
		number_of_bins			= bins;
		bin_observed_events		= std::vector<unsigned long int>(number_of_bins, 0);
		bin_expected_background = std::vector<double>(number_of_bins, 0.0);
		bin_efficiencies		= std::vector<double>(number_of_bins, 1.0);
	}
}

void DM_Detector::Set_Observed_Events(std::vector<unsigned long int> Ni)
{
	if(statistical_analysis != "Binned Poisson")
	{
		std::cerr << libphysica::Formatted_String("Error", "Red", true) << " in obscura::DM_Detector::Set_Observed_Events(std::vector<unsigned long int>): Statistical analysis is " << statistical_analysis << " not 'Binned Poisson'." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	else if(Ni.size() != number_of_bins)
	{
		std::cerr << libphysica::Formatted_String("Error", "Red", true) << " in obscura::DM_Detector::Set_Observed_Events(std::vector<unsigned long int>): Length of the input (" << Ni.size() << ") is not equal to the number of bins(" << number_of_bins << ")." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	else
	{
		bin_observed_events = Ni;
		observed_events		= std::accumulate(bin_observed_events.begin(), bin_observed_events.end(), 0);
	}
}

void DM_Detector::Set_Bin_Efficiencies(const std::vector<double>& eff)
{
	if(statistical_analysis != "Binned Poisson")
	{
		std::cerr << libphysica::Formatted_String("Error", "Red", true) << " in obscura::DM_Detector::Set_Bin_Efficiencies(const std::vector<double>&): Statistical analysis is " << statistical_analysis << " not 'Binned Poisson'." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	else if(eff.size() != number_of_bins)
	{
		std::cerr << libphysica::Formatted_String("Error", "Red", true) << " in obscura::DM_Detector::Set_Bin_Efficiencies(const std::vector<double>&): Length of the input (" << eff.size() << ") is not equal to the number of bins(" << number_of_bins << ")." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	else
	{
		bin_efficiencies = eff;
	}
}

void DM_Detector::Set_Expected_Background(const std::vector<double>& Bi)
{
	if(statistical_analysis != "Binned Poisson")
	{
		std::cerr << libphysica::Formatted_String("Error", "Red", true) << " in obscura::DM_Detector::Set_Expected_Background(const std::vector<double>&): Statistical analysis is " << statistical_analysis << " not 'Binned Poisson'." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	else if(Bi.size() != number_of_bins)
	{
		std::cerr << libphysica::Formatted_String("Error", "Red", true) << " in obscura::DM_Detector::Set_Expected_Background(const std::vector<double>&): Length of the input (" << Bi.size() << ") is not equal to the number of bins(" << number_of_bins << ")." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	else
	{
		bin_expected_background = Bi;
		expected_background		= std::accumulate(bin_expected_background.begin(), bin_expected_background.end(), 0.0);
	}
}

// (c) Maximum gap a'la Yellin
void DM_Detector::Use_Maximum_Gap(std::vector<double> energies)
{
	statistical_analysis = "Maximum Gap";

	maximum_gap_energy_data = energies;
	std::sort(maximum_gap_energy_data.begin(), maximum_gap_energy_data.end());
	energy_threshold = maximum_gap_energy_data.front();
	energy_max		 = maximum_gap_energy_data.back();
}

double CDF_Maximum_Gap(double x, double mu)
{
	if(x == mu)
		return 1.0 - exp(-mu);
	else
	{
		int m	   = mu / x;
		double sum = 0.0;
		for(int k = 0; k <= m; k++)
		{
			double term = pow(k * x - mu, k) / libphysica::Factorial(k) * exp(-k * x) * (1.0 + k / (mu - k * x));
			sum += term;
			if(fabs(term) < 1e-20)
				break;
		}
		return sum;
	}
}

double DM_Detector::P_Value_Maximum_Gap(DM_Particle& DM, DM_Distribution& DM_distr)
{
	// Interpolate the spectrum
	unsigned int interpolation_points = 400;
	std::vector<double> energies;
	double maximum_energy_deposit = Maximum_Energy_Deposit(DM, DM_distr);
	if(maximum_energy_deposit < energy_max)
	{
		energies = libphysica::Linear_Space(energy_threshold, maximum_energy_deposit, interpolation_points);
		energies.push_back(energy_max);
	}
	else
		energies = libphysica::Linear_Space(energy_threshold, energy_max, interpolation_points);
	std::vector<double> spectrum_values;
	for(auto& energy : energies)
		spectrum_values.push_back(exposure * dRdE(energy, DM, DM_distr));
	libphysica::Interpolation spectrum(energies, spectrum_values);

	// Determine all gaps and find the maximum.
	std::vector<double> gaps;
	for(unsigned int i = 0; i < (maximum_gap_energy_data.size() - 1); i++)
	{
		double E1  = maximum_gap_energy_data[i];
		double E2  = maximum_gap_energy_data[i + 1];
		double gap = spectrum.Integrate(E1, E2);
		gaps.push_back(gap);
	}

	double max_gap = *std::max_element(gaps.begin(), gaps.end());

	double N	   = std::accumulate(gaps.begin(), gaps.end(), 0.0);
	double p_value = 1.0 - CDF_Maximum_Gap(max_gap, N);
	return p_value;
}

void DM_Detector::Set_Flat_Efficiency(double eff)
{
	flat_efficiency = eff;
}

std::string DM_Detector::Target_Particles()
{
	return targets;
}

// DM functions
double DM_Detector::DM_Signals_Total(const DM_Particle& DM, DM_Distribution& DM_distr)
{
	double N = 0;
	if(statistical_analysis == "Binned Poisson")
	{
		std::vector<double> binned_events = DM_Signals_Binned(DM, DM_distr);
		N								  = std::accumulate(binned_events.begin(), binned_events.end(), 0.0);
	}
	else
	{
		std::vector<double> args = libphysica::Log_Space(energy_threshold, energy_max, 200);
		std::vector<double> values;
		for(auto& arg : args)
		{
			values.push_back(dRdE(arg, DM, DM_distr));
		}
		libphysica::Interpolation interpol(args, values);
		N = exposure * interpol.Integrate(energy_threshold, energy_max);
	}
	return N;
}

double DM_Detector::DM_Signal_Rate_Total(const DM_Particle& DM, DM_Distribution& DM_distr)
{
	return DM_Signals_Total(DM, DM_distr) / exposure;
}

std::vector<double> DM_Detector::DM_Signals_Binned(const DM_Particle& DM, DM_Distribution& DM_distr)
{
	if(statistical_analysis != "Binned Poisson")
	{
		std::cerr << libphysica::Formatted_String("Error", "Red", true) << " in obscura::DM_Detector::DM_Signals_Binned(const DM_Particle&, DM_Distribution&): The statistical analysis is " << statistical_analysis << ", not 'Binned Poisson'." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	else if(using_energy_bins)
		return DM_Signals_Energy_Bins(DM, DM_distr);
	else
	{
		std::cerr << libphysica::Formatted_String("Error", "Red", true) << " in obscura::DM_Detector::DM_Signals_Binned(): Statistical analysis is 'Binned Poisson' but no bins have been defined. This should not happen ever." << std::endl;
		std::exit(EXIT_FAILURE);
	}
}

// Limits/Constraints
double DM_Detector::Upper_Limit(DM_Particle& DM, DM_Distribution& DM_distr, double certainty)
{
	bool found_limit = true;

	double interaction_parameter_original = DM.Get_Interaction_Parameter(targets);
	if(statistical_analysis == "Binned Poisson" || statistical_analysis == "Poisson")
	{
		using_fiducial_values = true;
		fiducial_coupling	  = interaction_parameter_original;
		if(statistical_analysis == "Binned Poisson")
			fiducial_spectrum = DM_Signals_Binned(DM, DM_distr);
		else
			fiducial_signals = DM_Signals_Total(DM, DM_distr);
	}
	// Find the interaction parameter such that p = 1-certainty
	std::function<double(double)> func = [this, &DM, &DM_distr, certainty](double log10_parameter) {
		double parameter = pow(10.0, log10_parameter);
		DM.Set_Interaction_Parameter(parameter, targets);
		double p_value = P_Value(DM, DM_distr);
		return p_value - (1.0 - certainty);
	};
	double log10_upper_bound;
	if(func(-30.0) * func(10.0) > 0)
		found_limit = false;
	else
		log10_upper_bound = libphysica::Find_Root(func, -30.0, 10.0, 1.0e-4);

	DM.Set_Interaction_Parameter(interaction_parameter_original, targets);
	if(statistical_analysis == "Binned Poisson" || statistical_analysis == "Poisson")
	{
		using_fiducial_values = false;
		fiducial_coupling	  = 0.0;
		fiducial_signals	  = 0.0;
		fiducial_spectrum.clear();
	}
	if(found_limit)
		return pow(10.0, log10_upper_bound);
	else
		return -1.0;
}

std::vector<std::vector<double>> DM_Detector::Upper_Limit_Curve(DM_Particle& DM, DM_Distribution& DM_distr, std::vector<double> masses, double certainty)
{
	double mOriginal   = DM.mass;
	double lowest_mass = Minimum_DM_Mass(DM, DM_distr);
	std::vector<std::vector<double>> limit;

	for(unsigned int i = 0; i < masses.size(); i++)
	{
		if(masses[i] < lowest_mass)
			continue;
		DM.Set_Mass(masses[i]);
		double upper_limit = Upper_Limit(DM, DM_distr, certainty);
		if(upper_limit > 0.0)
			limit.push_back(std::vector<double> {masses[i], upper_limit});
	}
	DM.Set_Mass(mOriginal);
	return limit;
}

// Energy spectrum
void DM_Detector::Use_Energy_Threshold(double Ethr, double Emax)
{
	Initialize_Poisson();
	using_energy_threshold = true;
	energy_threshold	   = Ethr;
	energy_max			   = Emax;
	if(energy_max < energy_threshold)
	{
		std::cerr << libphysica::Formatted_String("Error", "Red", true) << " in obscura::DM_Detector::Use_Energy_Threshold(): Energy threshold (" << energy_threshold / keV << "keV) is higher than maximum energy (" << energy_max / keV << "keV)." << std::endl;
		std::exit(EXIT_FAILURE);
	}
}

void DM_Detector::Use_Energy_Bins(double Emin, double Emax, int bins)
{
	Initialize_Binned_Poisson(bins);
	using_energy_bins = true;
	energy_threshold  = Emin;
	energy_max		  = Emax;
	bin_energies	  = libphysica::Linear_Space(energy_threshold, energy_max, number_of_bins + 1);
	if(energy_max < energy_threshold)
	{
		std::cerr << libphysica::Formatted_String("Error", "Red", true) << " in obscura::DM_Detector::Use_Energy_Bins(): Energy threshold (" << energy_threshold / keV << "keV) is higher than maximum energy (" << energy_max / keV << "keV)." << std::endl;
		std::exit(EXIT_FAILURE);
	}
}

std::vector<double> DM_Detector::DM_Signals_Energy_Bins(const DM_Particle& DM, DM_Distribution& DM_distr)
{
	if(!using_energy_bins)
	{
		std::cerr << libphysica::Formatted_String("Error", "Red", true) << " in obscura::DM_Detector::DM_Signals_Energy_Bins(const DM_Particle&,DM_Distribution&): Not using energy bins." << std::endl;
		std::exit(EXIT_FAILURE);
	}
	else
	{
		std::function<double(double)> spectrum = [this, &DM, &DM_distr](double E) {
			return dRdE(E, DM, DM_distr);
		};
		std::vector<double> mu_i;
		for(unsigned int i = 0; i < number_of_bins; i++)
		{
			double mu = exposure * libphysica::Integrate(spectrum, bin_energies[i], bin_energies[i + 1]);
			mu_i.push_back(bin_efficiencies[i] * mu);
		}
		return mu_i;
	}
}

void DM_Detector::Print_Summary_Base(int MPI_rank) const
{
	if(MPI_rank == 0)
	{
		std::cout << std::endl
				  << "----------------------------------------" << std::endl
				  << "Experiment summary:\t" << name << std::endl
				  << "\tTarget particles:\t" << targets << std::endl
				  << "\tExposure [kg year]:\t" << libphysica::Round(In_Units(exposure, kg * year)) << std::endl
				  << "\tFlat efficiency [%]:\t" << libphysica::Round(100.0 * flat_efficiency) << std::endl
				  << "\tObserved events:\t" << observed_events << std::endl
				  << "\tExpected background:\t" << expected_background << std::endl
				  << "\tStatistical analysis:\t" << statistical_analysis << std::endl;
		if(statistical_analysis == "Binned Poisson")
		{
			std::cout << "\t\tNumber of bins:\t" << number_of_bins << std::endl;
			if(!bin_observed_events.empty() && !bin_observed_events.empty())
			{
				std::cout << "\t\tBin\tEfficiency[%]\tObserved events\tExpected background" << std::endl;
				for(unsigned int i = 0; i < bin_observed_events.size(); i++)
					std::cout << "\t\t" << i + 1 << "\t" << 100.0 * bin_efficiencies[i] << "\t\t" << bin_observed_events[i] << "\t\t" << bin_expected_background[i] << std::endl;
			}
		}
		if(using_energy_threshold || using_energy_bins || statistical_analysis == "Maximum Gap")
			std::cout << "\tRecoil energies [keV]:\t[" << libphysica::Round(energy_threshold / keV) << "," << libphysica::Round(energy_max / keV) << "]" << std::endl;
		if(using_energy_bins)
		{
			std::cout << "\n\t\tBin\tBin range [keV]" << std::endl;
			for(unsigned int bin = 0; bin < number_of_bins; bin++)
			{
				std::cout << "\t\t" << bin + 1 << "\t[" << libphysica::Round(In_Units(bin_energies[bin], keV)) << "," << libphysica::Round(In_Units(bin_energies[bin + 1], keV)) << ")" << std::endl;
			}
		}
		std::cout << std::endl;
	}
}

}	// namespace obscura
