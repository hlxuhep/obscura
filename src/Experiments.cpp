#include "obscura/Experiments.hpp"

#include "libphysica/Natural_Units.hpp"
#include "libphysica/Utilities.hpp"

namespace obscura
{
using namespace libphysica::natural_units;

// 1. Nuclear recoil experiments
DM_Detector_Nucleus DAMIC_N_2011()
{
	// Source: arXiv:1105.5191
	double DAMIC_exposure			   = 0.107 * kg * day;
	std::vector<Nucleus> DAMIC_targets = {Get_Nucleus(14)};
	double DAMIC_threshold			   = 0.55 * keV;
	double DAMIC_Emax				   = 7.0 * keV;
	unsigned int DAMIC_observed_events = 106;

	DM_Detector_Nucleus detector("DAMIC_N_2011", DAMIC_exposure, DAMIC_targets);
	detector.Use_Energy_Threshold(DAMIC_threshold, DAMIC_Emax);
	detector.Set_Observed_Events(DAMIC_observed_events);

	return detector;
}

DM_Detector_Nucleus XENON1T_N_2017()
{
	// Source: arXiv:1705.06655
	double XENON1T_exposure				 = 34.2 * day * 1042 * kg;
	std::vector<Nucleus> XENON1T_targets = {Get_Nucleus(54)};
	double XENON1T_threshold			 = 5.0 * keV;
	double XENON1T_Emax					 = 40.0 * keV;
	double XENON1T_efficiency			 = 0.82;
	unsigned int XENON1T_observed_events = 0;

	DM_Detector_Nucleus detector("XENON1T_N_2017", XENON1T_exposure, XENON1T_targets);
	detector.Set_Flat_Efficiency(XENON1T_efficiency);
	detector.Use_Energy_Threshold(XENON1T_threshold, XENON1T_Emax);
	detector.Set_Observed_Events(XENON1T_observed_events);

	return detector;
}

DM_Detector_Nucleus CRESST_II()
{
	// Source: arXiv:1509.01515 and arXiv:1701.08157
	double CRESST_II_exposure					= 52.15 * kg * day;
	std::vector<Nucleus> CRESST_II_targets		= {Get_Nucleus(8), Get_Nucleus(20), Get_Nucleus(74)};	// CaOW
	std::vector<double> CRESST_II_target_ratios = {4, 1, 1};
	double CRESST_II_threshold					= 307 * eV;
	double CRESST_II_Emax						= 40.0 * keV;
	double CRESST_II_resolution					= CRESST_II_threshold / 5.0;
	std::vector<std::string> efficiency_files	= {PROJECT_DIR "data/CRESST-II/Lise_eff_AR_O.dat", PROJECT_DIR "data/CRESST-II/Lise_eff_AR_Ca.dat", PROJECT_DIR "data/CRESST-II/Lise_eff_AR_W.dat"};

	DM_Detector_Nucleus detector("CRESST-II", CRESST_II_exposure, CRESST_II_targets, CRESST_II_target_ratios);
	std::vector<double> energy_events = libphysica::Import_List(PROJECT_DIR "data/CRESST-II/Lise_AR.dat", keV);
	energy_events.push_back(CRESST_II_threshold);
	energy_events.push_back(CRESST_II_Emax);
	detector.Use_Maximum_Gap(energy_events);
	detector.Set_Resolution(CRESST_II_resolution);
	detector.Import_Efficiency(efficiency_files, keV);

	return detector;
}

DM_Detector_Nucleus CRESST_III()
{
	// Source: arXiv:1711.07692 and arXiv:1905.07335
	double CRESST_III_exposure					 = 5.594 * kg * day;
	std::vector<Nucleus> CRESST_III_targets		 = {Get_Nucleus(8), Get_Nucleus(20), Get_Nucleus(74)};	 // CaOW
	std::vector<double> CRESST_III_target_ratios = {4, 1, 1};
	double CRESST_III_threshold					 = 30.1 * eV;
	double CRESST_III_Emax						 = 16 * keV;
	double CRESST_III_resolution				 = 4.6 * eV;
	double CRESST_III_efficiency				 = 0.5;
	std::vector<std::string> efficiency_files	 = {PROJECT_DIR "data/CRESST-III/C3P1_DetA_eff_AR_O.dat", PROJECT_DIR "data/CRESST-III/C3P1_DetA_eff_AR_Ca.dat", PROJECT_DIR "data/CRESST-III/C3P1_DetA_eff_AR_W.dat"};

	DM_Detector_Nucleus detector("CRESST-III", CRESST_III_exposure, CRESST_III_targets, CRESST_III_target_ratios);
	detector.Set_Flat_Efficiency(CRESST_III_efficiency);
	std::vector<double> energy_events = libphysica::Import_List(PROJECT_DIR "data/CRESST-III/C3P1_DetA_AR.dat", keV);
	energy_events.push_back(CRESST_III_threshold);
	energy_events.push_back(CRESST_III_Emax);
	detector.Use_Maximum_Gap(energy_events);
	detector.Set_Resolution(CRESST_III_resolution);
	detector.Import_Efficiency(efficiency_files, keV);

	return detector;
}

DM_Detector_Nucleus CRESST_surface()
{
	// Source: arXiv:1707.06749
	double CRESST_surface_exposure					 = 0.046 * gram * day;
	std::vector<Nucleus> CRESST_surface_targets		 = {Get_Nucleus(8), Get_Nucleus(13)};
	std::vector<double> CRESST_surface_target_ratios = {3, 2};
	double CRESST_surface_threshold					 = 19.7 * eV;
	double CRESST_surface_Emax						 = 600 * eV;
	double CRESST_surface_resolution				 = 3.74 * eV;

	DM_Detector_Nucleus detector("CRESST-surface", CRESST_surface_exposure, CRESST_surface_targets, CRESST_surface_target_ratios);
	std::vector<double> energy_events = libphysica::Import_List(PROJECT_DIR "data/CRESST-surface/data.txt", keV);
	energy_events.push_back(CRESST_surface_threshold);
	energy_events.push_back(CRESST_surface_Emax);
	detector.Use_Maximum_Gap(energy_events);
	detector.Set_Resolution(CRESST_surface_resolution);

	return detector;
}

// 2. Electron recoil experiments - Ionization
DM_Detector_Ionization_ER XENON10_S2_ER()
{
	// Source: arXiv:1104.3088, arXiv:1206.2644, and arXiv:1703.00910
	std::string target_name							   = "Xe";
	double exposure									   = 15 * kg * day;
	double flat_efficiency							   = 0.92;
	std::vector<unsigned long int> observed_event_bins = {126, 60, 12, 3, 2, 0, 2};
	double muPE										   = 27.0;
	double sigPE									   = 6.7;
	std::vector<unsigned int> S2_bin_ranges			   = {14, 41, 68, 95, 122, 149, 176, 203};
	std::string trigger_efficiency					   = PROJECT_DIR "data/XENON10e/PE_Trigger_Efficiency.txt";

	DM_Detector_Ionization_ER detector("XENON10_S2", exposure, target_name);
	detector.Set_Flat_Efficiency(flat_efficiency);
	detector.Initialize_S2_Spectrum("Poisson+Gauss", muPE, sigPE);
	detector.Use_PE_Bins(S2_bin_ranges);
	detector.Set_Observed_Events(observed_event_bins);
	detector.Import_Trigger_Efficiency_PE(trigger_efficiency);

	return detector;
}

DM_Detector_Ionization_ER XENON100_S2_ER()
{
	// Source: arXiv:1605.06262, arXiv:1703.00910
	std::string target_name							   = "Xe";
	double exposure									   = 30 * kg * year;
	std::vector<unsigned long int> observed_event_bins = {794, 1218, 924, 776, 669, 630, 528, 488, 433, 387};
	double muPE										   = 19.7;
	double sigPE									   = 6.2;
	std::vector<unsigned int> S2_bin_ranges			   = {80, 90, 110, 130, 150, 170, 190, 210, 230, 250, 270};
	std::string trigger_efficiency					   = PROJECT_DIR "data/XENON100e/PE_Trigger_Efficiency.txt";
	std::string acceptance_efficiency				   = PROJECT_DIR "data/XENON100e/PE_Acceptance_Efficiency.txt";

	DM_Detector_Ionization_ER detector("XENON100_S2", exposure, target_name);
	detector.Initialize_S2_Spectrum("Poisson+Gauss", muPE, sigPE);
	detector.Use_PE_Bins(S2_bin_ranges);
	detector.Set_Observed_Events(observed_event_bins);
	detector.Import_Trigger_Efficiency_PE(trigger_efficiency);
	detector.Import_Acceptance_Efficiency_PE(acceptance_efficiency);

	return detector;
}

DM_Detector_Ionization_ER XENON1T_S2_ER()
{
	// Source: arXiv:1907.11485
	std::string target_name = "Xe";
	// double exposure			= 80755.2 * kg * day;
	double exposure = 0.97678 * tonne * year;
	double muPE		= 33.0;
	double sigPE	= 7.0;
	// std::vector<unsigned long int> observed_event_bins = {8, 7, 3, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 1, 1, 1, 2, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 2, 1, 0, 1, 1, 1, 1, 0, 0, 3, 1};
	// std::vector<unsigned int> S2_bin_ranges = {150, 200, 250, 300, 350, 400, 450, 500, 550, 600, 650, 700, 750, 800, 850, 900, 950, 1000, 1050, 1100, 1150, 1200, 1250, 1300, 1350, 1400, 1450, 1500, 1550, 1600, 1650, 1700, 1750, 1800, 1850, 1900, 1950, 2000, 2050, 2100, 2150, 2200, 2250, 2300, 2350, 2400, 2450, 2500, 2550, 2600, 2650, 2700, 2750, 2800, 2850, 2900, 2950, 3000};
	std::vector<unsigned long int> observed_event_bins = {21, 1, 11, 12};
	std::vector<unsigned int> S2_bin_ranges			   = {150, 863, 1575, 2288, 3000};
	std::vector<double> binned_background_expectation  = {4.74353, 2.01263, 3.36622, 4.01199};
	std::string trigger_efficiency					   = PROJECT_DIR "data/XENON1T_S2/XENON1T_TotalEfficiency.txt";

	DM_Detector_Ionization_ER detector("XENON1T_S2", exposure, target_name);
	// detector.Initialize_S2_Spectrum("Poisson+Gauss", muPE, sigPE);
	detector.Initialize_S2_Spectrum("Response matrix");
	detector.Use_PE_Bins(S2_bin_ranges);
	detector.Set_Observed_Events(observed_event_bins);
	detector.Set_Expected_Background(binned_background_expectation);
	// detector.Import_Trigger_Efficiency_PE(trigger_efficiency);
	return detector;
}

DM_Detector_Ionization_ER DarkSide50_S2_ER()
{
	// Source: arXiv:1802.06998
	std::string target_name							   = "Ar";
	double exposure									   = 6786.0 * kg * day;
	unsigned int ne_threshold						   = 3;
	std::vector<unsigned long int> observed_event_bins = {6131, 673, 252, 227, 198, 199, 189, 247, 230, 261, 249, 329, 336};

	DM_Detector_Ionization_ER detector("DarkSide-50_S2", exposure, target_name);
	detector.Use_Electron_Bins(ne_threshold, 13);
	detector.Set_Observed_Events(observed_event_bins);

	return detector;
}

// 3. Electron recoil experiments - Semiconductor
DM_Detector_Crystal protoSENSEI_at_Surface()
{
	// Source: arXiv:1804.00088
	double SENSEI_surface_exposure								  = 0.07 * gram * 456 * minute;
	unsigned int SENSEI_surface_Q_threshold						  = 1;
	unsigned int SENSEI_surface_N_bins							  = 5;
	std::vector<double> SENSEI_surface_efficiencies				  = {0.668, 0.41, 0.32, 0.27, 0.24};
	std::vector<unsigned long int> SENSEI_surface_observed_events = {140302, 4676, 131, 1, 0};

	DM_Detector_Crystal detector("protoSENSEI@surface", SENSEI_surface_exposure, "Si");
	detector.Use_Q_Bins(SENSEI_surface_Q_threshold, SENSEI_surface_N_bins);
	detector.Set_Observed_Events(SENSEI_surface_observed_events);
	detector.Set_Bin_Efficiencies(SENSEI_surface_efficiencies);

	return detector;
}

DM_Detector_Crystal protoSENSEI_at_MINOS()
{
	// Source: arXiv:1901.10478
	double SENSEI_exposure								  = 0.246 * gram * day;
	unsigned int SENSEI_Q_threshold						  = 1;
	unsigned int SENSEI_N_bins							  = 3;
	std::vector<double> SENSEI_efficiencies				  = {1.0, 0.62, 0.48};
	std::vector<unsigned long int> SENSEI_observed_events = {8516, 87, 0};

	DM_Detector_Crystal detector("protoSENSEI@MINOS", SENSEI_exposure, "Si");
	detector.Use_Q_Bins(SENSEI_Q_threshold, SENSEI_N_bins);
	detector.Set_Observed_Events(SENSEI_observed_events);
	detector.Set_Bin_Efficiencies(SENSEI_efficiencies);

	return detector;
}

DM_Detector_Crystal SENSEI_at_MINOS()
{
	// Source: arXiv:2004.11378
	double SENSEI_exposure								  = 9.1 * gram * day;
	unsigned int SENSEI_Q_threshold						  = 1;
	unsigned int SENSEI_N_bins							  = 4;
	std::vector<double> SENSEI_efficiencies				  = {1.38 / 9.1, 2.09 / 9.1, 9.03 / 9.1, 1.0};
	std::vector<unsigned long int> SENSEI_observed_events = {578, 5, 0, 0};

	DM_Detector_Crystal detector("SENSEI@MINOS", SENSEI_exposure, "Si");
	detector.Use_Q_Bins(SENSEI_Q_threshold, SENSEI_N_bins);
	detector.Set_Observed_Events(SENSEI_observed_events);
	detector.Set_Bin_Efficiencies(SENSEI_efficiencies);

	return detector;
}

DM_Detector_Crystal CDMS_HVeV_2018()
{
	// Source: arXiv:1804.10697
	double SuperCDMS_exposure								 = 0.487 * gram * day;
	double SuperCDMS_flat_efficiency						 = 0.9545;
	unsigned int SuperCDMS_Q_threshold						 = 1;
	unsigned int SuperCDMS_N_bins							 = 6;
	std::vector<double> SuperCDMS_efficiencies				 = {0.88, 0.91, 0.91, 0.91, 0.91, 0.91};
	std::vector<unsigned long int> SuperCDMS_observed_events = {53000, 400, 74, 18, 7, 14};

	DM_Detector_Crystal detector("CDMS-HVeV_2018", SuperCDMS_exposure, "Si");
	detector.Set_Flat_Efficiency(SuperCDMS_flat_efficiency);
	detector.Use_Q_Bins(SuperCDMS_Q_threshold, SuperCDMS_N_bins);
	detector.Set_Observed_Events(SuperCDMS_observed_events);
	detector.Set_Bin_Efficiencies(SuperCDMS_efficiencies);

	return detector;
}

DM_Detector_Crystal CDMS_HVeV_2020()
{
	// Source: arXiv:2005.14067
	double SuperCDMS_exposure								 = 1.2 * gram * day;
	unsigned int SuperCDMS_Q_threshold						 = 1;
	unsigned int SuperCDMS_N_bins							 = 6;
	std::vector<unsigned long int> SuperCDMS_observed_events = {178800, 1320, 248, 64, 19, 6};

	DM_Detector_Crystal detector("CDMS-HVeV_2020", SuperCDMS_exposure, "Si");
	detector.Use_Q_Bins(SuperCDMS_Q_threshold, SuperCDMS_N_bins);
	detector.Set_Observed_Events(SuperCDMS_observed_events);

	return detector;
}

// 4. Migdal experiments - Ionization
DM_Detector_Ionization_Migdal XENON10_S2_Migdal()
{
	// Source: arXiv:1104.3088, arXiv:1206.2644, and arXiv:1703.00910
	std::string target_name							   = "Xe";
	double exposure									   = 15 * kg * day;
	double flat_efficiency							   = 0.92;
	std::vector<unsigned long int> observed_event_bins = {126, 60, 12, 3, 2, 0, 2};
	double muPE										   = 27.0;
	double sigPE									   = 6.7;
	std::vector<unsigned int> S2_bin_ranges			   = {14, 41, 68, 95, 122, 149, 176, 203};
	std::string trigger_efficiency					   = PROJECT_DIR "data/XENON10e/PE_Trigger_Efficiency.txt";

	DM_Detector_Ionization_Migdal detector("XENON10_S2", exposure, target_name);
	detector.Set_Flat_Efficiency(flat_efficiency);
	detector.Initialize_S2_Spectrum("Poisson+Gauss", muPE, sigPE);
	detector.Use_PE_Bins(S2_bin_ranges);
	detector.Set_Observed_Events(observed_event_bins);
	detector.Import_Trigger_Efficiency_PE(trigger_efficiency);

	return detector;
}

DM_Detector_Ionization_Migdal XENON100_S2_Migdal()
{
	// Source: arXiv:1605.06262, arXiv:1703.00910
	std::string target_name							   = "Xe";
	double exposure									   = 30 * kg * year;
	std::vector<unsigned long int> observed_event_bins = {794, 1218, 924, 776, 669, 630, 528, 488, 433, 387};
	double muPE										   = 19.7;
	double sigPE									   = 6.2;
	std::vector<unsigned int> S2_bin_ranges			   = {80, 90, 110, 130, 150, 170, 190, 210, 230, 250, 270};
	std::string trigger_efficiency					   = PROJECT_DIR "data/XENON100e/PE_Trigger_Efficiency.txt";
	std::string acceptance_efficiency				   = PROJECT_DIR "data/XENON100e/PE_Acceptance_Efficiency.txt";

	DM_Detector_Ionization_Migdal detector("XENON100_S2", exposure, target_name);
	detector.Initialize_S2_Spectrum("Poisson+Gauss", muPE, sigPE);
	detector.Use_PE_Bins(S2_bin_ranges);
	detector.Set_Observed_Events(observed_event_bins);
	detector.Import_Trigger_Efficiency_PE(trigger_efficiency);
	detector.Import_Acceptance_Efficiency_PE(acceptance_efficiency);

	return detector;
}

DM_Detector_Ionization_Migdal XENON1T_S2_Migdal()
{
	// Source: arXiv:1907.11485
	std::string target_name							   = "Xe";
	double exposure									   = 80755.2 * kg * day;
	std::vector<unsigned long int> observed_event_bins = {8, 7, 3, 0};
	double muPE										   = 33.0;
	double sigPE									   = 7.0;
	std::vector<unsigned int> S2_bin_ranges			   = {150, 200, 250, 300, 350};
	std::string trigger_efficiency					   = PROJECT_DIR "data/XENON1T_S2/XENON1T_TotalEfficiency.txt";

	DM_Detector_Ionization_Migdal detector("XENON1T_S2", exposure, target_name);
	detector.Initialize_S2_Spectrum("Poisson+Gauss", muPE, sigPE);
	detector.Use_PE_Bins(S2_bin_ranges);
	detector.Set_Observed_Events(observed_event_bins);
	detector.Import_Trigger_Efficiency_PE(trigger_efficiency);

	return detector;
}

DM_Detector_Ionization_Migdal DarkSide50_S2_Migdal()
{
	// Source: arXiv:1802.06998
	std::string target_name							   = "Ar";
	double exposure									   = 6786.0 * kg * day;
	unsigned int ne_threshold						   = 3;
	std::vector<unsigned long int> observed_event_bins = {6131, 673, 252, 227, 198, 199, 189, 247, 230, 261, 249, 329, 336};

	DM_Detector_Ionization_Migdal detector("DarkSide-50_S2", exposure, target_name);
	detector.Use_Electron_Bins(ne_threshold, 13);
	detector.Set_Observed_Events(observed_event_bins);

	return detector;
}

}	// namespace obscura