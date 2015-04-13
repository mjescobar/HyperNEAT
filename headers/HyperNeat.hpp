#ifndef HYPERNEAT_H
#define HYPERNEAT_H

#include <NEATSPIKES>
#include "Substrate.hpp"
#include "CPPNInputs.hpp"
#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>

using namespace std;
using namespace NEATSpikes;
/**
 * \namespace ANN_USM
 * \brief Dedicated to artificial intelligence development in Santa María University.
 */
namespace ANN_USM{
	/**
	 * \class HyperNeat
	 * \brief The HyperNeat class is used to implement a neuroevolution method called HyperNeat
	 */
	class HyperNeat
	{
		// Vector of aditional cppn inputs
		vector < CPPNInputs > AditionalCPPNInputs;

		//Threshold that determine the creation for an connection
		double connection_threshold;

		bool OkConnections;

		char * test_name;

		Substrate * substrate;/**< hyperNeat substrate */

		GlobalInformation * ilo;
		BasicSynapticWeight * BSW;
		BasicNeuron * BN;
		Life NeatPopulation;
		vector < Organism * > OrganismGeneration;

	public:

		/**
		 * \brief Constructor with parameters
		 * \param inputs Input vector
		 * \param outputs Output vector
		 * \param hyperneat_info_file Json file
		 */
		HyperNeat(vector < double * > inputs, vector < double * > outputs, char * path1, char * path2);
		/**
		 * \brief Destructor
		 */
		~HyperNeat();
		/**
		 * \brief Extract all HyperNeat information of json string
		 * \param hyperneat_info json string
		 */
		void HJsonDeserialize(string hyperneat_info);
		/**
		 * \brief Create all substrate connections according to CPPN-NEAT result
		 * \param organism_id Number of population organism  in CPPN-NEAT that will create connections in the substrate
		 * \return The return value is true if the creation of connections was successful and false if it was not
		 */
		bool CreateSubstrateConnections(int organism_id);
		bool CreateSubstrateConnections(char * path);
		/**
		 * \brief Allows to obtain the final HyperNeat outputs
		 * \return The return value is true if the evaluation of connections was successful and false if it was not
		 */
		bool EvaluateSubstrateConnections();
		/**
		 * \brief Set CPPN-NEAT fitness of last interation
		 * \param fitness Fitnnes value to set
		 * \param organism_id Number of population organism in CPPN-NEAT that created the connections in the substrate
		 * \return The return value is true if the fitness correspond to a champ organism and false if it was not
		 */
		bool HyperNeatFitness(double fitness, int organism_id);
		/**
		 * \brief Allows evolve cppn_neat
		 */
		void HyperNeatEvolve();		
		/**
		 * \brief Allows obtain all final functions of every output node
		 * \param plataform Name of destination plataform for HyperNeat functions
		 */
		void GetHyperNeatOutputFunctions(string plataform);

		void CreateOrganismPopulation();

		int getPopulationSize();

		int getGenerationSize();

	};
}
#endif