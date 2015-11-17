# CMS_GEM_TB_Timing
# ========================================================
	Software tools for the analysis of timing data collected at CMS GEM test beams

    Designed to work on lxplus running slc6 with ROOT version 6.00.02 and g++ version 4.8.4.

# Installation Instructions
# ========================================================
	Navigate to the scripts directory:

		cd scripts/

	Run the setup_CMS_GEM.sh script:

		source setup_CMS_GEM.csh	#(csh)
		source setup_CMS_GEM.sh		#(bash)

	Navigate to the source directory:

		cd ../src/

	Open makefile_plotter, ensure the compiler is correct for your system

		i.e. CC = g++ or clang (for Mac OS > 10.9 ???)

	Perform the same check for makefile_producer

	Build the project:

		make -f makefile_plotter	#(Tree Plotter)
		make -f makefile_producer	#(Tree Producer)
		make -f makefile_singleRunAna	#(Single Run analyzer)

	This will create three executables "plotTree," "produceTree," and "singleRunAnalyzer"
	
# Usage - plotTree
# ========================================================

	On new shell execute the setup script:

		source scripts/setup_CMS_GEM.csh	#(csh)
		source scripts/setup_CMS_GEM.sh		#(bash)

	To run the analysis:

		./src/plotTree <plot_list_file>.txt

	Several sample <plot_list_files>.txt have been included in this repository, for example:

		./src/plotTree lists_plots/PlotList_GE11_IV_iMSPL_iT.txt

	will generate an output file "PlotList_GE11_IV_iMSPL_iT.root" containing the prepared results.

	Please see "lists_plots/PlotList_Template.txt" for an explanation of the expected input and available options

# Usage - produceTree
# ========================================================

    On new shell execute the setup script:

        source scripts/setup_CMS_GEM.csh	#(csh)
        source scripts/setup_CMS_GEM.sh		#(bash)

    To run the producer:

        ./src/produceTree <Input_Production_Config_File> <Verbose Mode true/false>

    However you will first need to setup several text files.  The primary file is
    the "Input_Production_Config_File" mentioned above; a sample can be found at:

        setup_files/Production_ConfigFile_Template.txt

    Or the help of the producer will output a sample:

        ./src/produceTree -h

    Inside your Production_ConfigFile.txt file you will need to supply the physical filepath (PFP)
    of the raw data you would like to create a tree with.  You will need to supply the physical
    filename (PFN) of the run list file.  The run list file is a tab deliminted list of filenames.
    There should be one filename per line and then after a tab the name of the TreeSetupFile that
    should be used to parse the data in the root file.  If no TreeSetupFile is provided for a given
    run the values from the last TreeSetupFile read into memory during the processing is used.  This
    is advantageous since you may want to analyze 2000+ runs but only give the setup file a few times
    (i.e. whenever a hardware change occurs).  An example of the RunList File is provided as:

        lists_runs/RunList_Template.txt

    It is assumed that each filename in the RunList file follows an organized and consistent naming
    convention which is then defined in the TreeSetupFile, i.e.:

        TDCRun{XXXX}_{BeamType}_{EvtNo}k_MSPL{Y}_{TrigMode}_DLY{ZZ}ns_{RunType}_{GE11Currents}_{GE11Thresholds}.root

    The naming convention should be consistent over the data taking period for readability however
    the parsing algorithm is flexibile so small changes/typos are not a problem.  Also note that
    the naming convention is expected to be parameters separated by underscores.

    Note you should give ONLY the name of the data file, the PFP you provide in the
    Production_ConfigFile.txt will be used to find the file (i.e. providing the PFP in both places
    will cause the program to not find the file).  Then the TreeSetupFile should be given by its
    PFN (i.e. "setup_files/TreeSetupFile_Template.txt")

    An example of the TreeSetupFile is given in:

        setup_files/TreeSetupFile_Template.txt

    In this example critical information as comments are given at the start.  Please review this
    information before starting.

    In the Production_ConfigFile.txt the user must then give the PFN of the output root file and the
    name of the TTree to be produced inside this file.  Then several VerboseMode true/false flags are
    enabled to get important debugging information at runtime.  However, the true/false Verbose Mode
    entered at command line (mentioned above) is for the produceTree.cpp script and the true/false
    flags available in the Production_ConfigFile.txt correspond to the treeProducerTDC class.

    The last part of the Production_ConfigFile.txt are a list of strings in the filename that should
    be ignored because they could cause the parsing algorithm to crash or have undefined behavior.

# Usage - singleRunAnalyzer
# ========================================================

    On new shell execute the setup script:

        source scripts/setup_CMS_GEM.csh	#(csh)
        source scripts/setup_CMS_GEM.sh		#(bash)

    To run the single run analyzer:

        ./src/singleRunAnalyzer <Physical_Filename_Of_Input_ROOT_File> <Name_Of_TTree_Inside_Input_ROOT_File> <Analysis_Config_File> <Verbose Mode true/false>

    However you will first need to setup the analysis config text file; a sample 
    can be found at:

        setup_files/Analysis_ConfigFile_Template.txt

