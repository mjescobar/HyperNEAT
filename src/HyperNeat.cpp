#ifndef HYPERNEAT_CPP
#define HYPERNEAT_CPP

#include "HyperNeat.hpp"
using namespace ANN_USM;

HyperNeat::HyperNeat(vector < double * > inputs, vector < double * > outputs, char * path1, char * path2, char * path3)
{
	// ============================= READING JSON FILE ============================= //
	ifstream file;
	file.open(path1);
	string hyperneat_info((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
	// ============================================================================= //	

	substrate = new Substrate(inputs,outputs);

	char neatname[] = "NEAT";
	char ruta[] = "./NEAT_organisms/";

	cppn_neat = new Population(path2, path3, neatname, ruta);
	
	HJsonDeserialize(hyperneat_info);

	clog << "\t-> Deserialize ok!" << endl;
}
HyperNeat::~HyperNeat()
{
	free(substrate);
	free(cppn_neat);
	vector<CPPNInputs>().swap(AditionalCPPNInputs);
}

void HyperNeat::HJsonDeserialize(string hyperneat_info)
{
	char str[(int)hyperneat_info.size()];
	strcpy(str, hyperneat_info.c_str());
	const char delimeters[] = "{\"\t\n:,[ ]}";
	char *pch = strtok(str, delimeters);

	if(!strcmp(pch,(char *)"NAME_TEST"))
	{
		pch = strtok(NULL, delimeters);
		int size = strlen(pch)+1;
		test_name = (char *) malloc(size);
		strncpy(test_name,pch,size);
		pch = strtok(NULL, delimeters);
	}
	if(!strcmp(pch,(char *)"n_AditionalCPPNInputs"))
	{
		pch = strtok(NULL, delimeters);
		int n_AditionalCPPNInputs = atoi(pch);
		pch = strtok(NULL, delimeters);

		if(!strcmp(pch,(char *)"AditionalCPPNInputs"))
		{
			for(int i = 0; i < n_AditionalCPPNInputs; i++)
			{
				pch = strtok(NULL, delimeters);		
				if (!strcmp(pch,(char *)"BIAS"))
				{	
					char * aux = pch;
					pch = strtok(NULL, delimeters);						
					AditionalCPPNInputs.push_back(CPPNInputs(aux, atof(pch)));
				}else																
					AditionalCPPNInputs.push_back(CPPNInputs(pch, 0.0));
			}						
			pch = strtok(NULL, delimeters);					
		}
	}
	
	if(!strcmp(pch,(char *)"connection_threshold"))
	{
		pch = strtok(NULL, delimeters);
		connection_threshold = atof(pch);
		pch = strtok(NULL, delimeters);
	}
	if (!strcmp(pch,(char *)"Substrate"))
	{	
		pch = strtok(NULL, delimeters);
		pch = substrate->SJsonDeserialize(pch);
	}
}

bool HyperNeat::CreateSubstrateConnections(int organism_id)
{
	OkConnections = false;

	if(substrate->GetLayersNumber() == 0)
	{
		cout << "ERROR: Does not exist any substrate initialized" << endl;
		return false;
	}

	if((int)cppn_neat->champion.getNEATOutputSize() > 1 )
	{
		if(substrate->GetLayersNumber() > 2)
		{
			if((int)cppn_neat->champion.getNEATOutputSize() != substrate->GetLayersNumber()-1)
			{
				cout << "ERROR: The layout number does not correspond to the cppn output number" << endl;
				return false;
			}

			for(int i = 0; i < substrate->GetLayersNumber()-1; i++)
			{				
				substrate->ClearSpatialNodeInputs(i+1);

				for(int j = 0; j < substrate->GetLayerNodesNumber(i); j++)
					for(int k = 0; k < substrate->GetLayerNodesNumber(i+1); k++)
					{
						vector < double > cppn_inputs;
						vector < double > cppn_output;
						vector < double > c1 = (substrate->GetSpatialNode(i,j))->GetCoordenates();
						vector < double > c2 = (substrate->GetSpatialNode(i+1,k))->GetCoordenates();
						cppn_inputs.insert(cppn_inputs.end(), c1.begin(), c1.end());
						cppn_inputs.insert(cppn_inputs.end(), c2.begin(), c2.end());

						for(int c = 0; c < (int)AditionalCPPNInputs.size(); c++)
							cppn_inputs.push_back(AditionalCPPNInputs.at(c).Eval(cppn_inputs));

						if(organism_id == -1) cppn_output = cppn_neat->champion.eval(cppn_inputs);
						else cppn_output = cppn_neat->organisms.at(organism_id).eval(cppn_inputs);

						if(abs(cppn_output.at(i)) > connection_threshold)
							(substrate->GetSpatialNode(i+1,k))->AddInputToNode(substrate->GetSpatialNode(i,j), cppn_output.at(i));
						
					}
			}		
			
		}
		else
		{			
				cout << "ERROR: The layer number must be greater than two to use multiple cppn-neat outputs" << endl;
				return false;
		}

	}
	else
	{
		for(int i = 0; i < substrate->GetLayersNumber()-1; i++)
		{
			substrate->ClearSpatialNodeInputs(i+1);

			for(int j = 0; j < substrate->GetLayerNodesNumber(i); j++)
			{
				for(int k = 0; k < substrate->GetLayerNodesNumber(i+1); k++)
				{
					vector < double > cppn_inputs;
					double weight = 0.0;
					vector < double > c1 = (substrate->GetSpatialNode(i,j))->GetCoordenates();
					vector < double > c2 = (substrate->GetSpatialNode(i+1,k))->GetCoordenates();
					cppn_inputs.insert(cppn_inputs.end(), c1.begin(), c1.end());
					cppn_inputs.insert(cppn_inputs.end(), c2.begin(), c2.end());

					for(int c = 0; c < (int)AditionalCPPNInputs.size(); c++)
						cppn_inputs.push_back(AditionalCPPNInputs.at(c).Eval(cppn_inputs));

					if(organism_id == -1) weight = (cppn_neat->champion.eval(cppn_inputs)).at(0);
					else weight = (cppn_neat->organisms.at(organism_id).eval(cppn_inputs)).at(0);

					if(abs(weight) > connection_threshold)
						(substrate->GetSpatialNode(i+1,k))->AddInputToNode(substrate->GetSpatialNode(i,j), weight);
				}
			}
		}		
	}	

	for(int i = 0; i < substrate->GetLayersNumber(); i++)
		for(int j = 0; j < substrate->GetLayerNodesNumber(i); j++)
			if((substrate->GetSpatialNode(i,j))->GetNodeType() == 2 && (substrate->GetSpatialNode(i,j))->ActiveNode())
			{
				OkConnections = true;
				return true; 
			}
				

	cerr << "ERROR: Does not exist any active output node" << endl;
	return false;
	
}

bool HyperNeat::CreateSubstrateConnections(char * path)
{
	OkConnections = false;

	Genetic_Encoding organism;
	organism.load(path);

	if(substrate->GetLayersNumber() == 0)
	{
		cout << "ERROR: Does not exist any substrate initialized" << endl;
		return false;
	}

	if((int)cppn_neat->champion.getNEATOutputSize() > 1 )
	{
		if(substrate->GetLayersNumber() > 2)
		{
			if((int)cppn_neat->champion.getNEATOutputSize() != substrate->GetLayersNumber()-1)
			{
				cout << "ERROR: The layout number does not correspond to the cppn output number" << endl;
				return false;
			}

			for(int i = 0; i < substrate->GetLayersNumber()-1; i++)
			{				
				substrate->ClearSpatialNodeInputs(i+1);

				for(int j = 0; j < substrate->GetLayerNodesNumber(i); j++)
					for(int k = 0; k < substrate->GetLayerNodesNumber(i+1); k++)
					{
						vector < double > cppn_inputs;
						vector < double > cppn_output;
						vector < double > c1 = (substrate->GetSpatialNode(i,j))->GetCoordenates();
						vector < double > c2 = (substrate->GetSpatialNode(i+1,k))->GetCoordenates();
						cppn_inputs.insert(cppn_inputs.end(), c1.begin(), c1.end());
						cppn_inputs.insert(cppn_inputs.end(), c2.begin(), c2.end());

						for(int c = 0; c < (int)AditionalCPPNInputs.size(); c++)
							cppn_inputs.push_back(AditionalCPPNInputs.at(c).Eval(cppn_inputs));

						cppn_output = organism.eval(cppn_inputs);

						if(abs(cppn_output.at(i)) > connection_threshold)
							(substrate->GetSpatialNode(i+1,k))->AddInputToNode(substrate->GetSpatialNode(i,j), cppn_output.at(i));
						
					}
			}		
			
		}
		else
		{			
				cout << "ERROR: The layer number must be greater than two to use multiple cppn-neat outputs" << endl;
				return false;
		}

	}
	else
	{
		for(int i = 0; i < substrate->GetLayersNumber()-1; i++)
		{
			substrate->ClearSpatialNodeInputs(i+1);

			for(int j = 0; j < substrate->GetLayerNodesNumber(i); j++)
			{
				for(int k = 0; k < substrate->GetLayerNodesNumber(i+1); k++)
				{
					vector < double > cppn_inputs;
					double weight = 0.0;
					vector < double > c1 = (substrate->GetSpatialNode(i,j))->GetCoordenates();
					vector < double > c2 = (substrate->GetSpatialNode(i+1,k))->GetCoordenates();
					cppn_inputs.insert(cppn_inputs.end(), c1.begin(), c1.end());
					cppn_inputs.insert(cppn_inputs.end(), c2.begin(), c2.end());

					for(int c = 0; c < (int)AditionalCPPNInputs.size(); c++)
						cppn_inputs.push_back(AditionalCPPNInputs.at(c).Eval(cppn_inputs));
					
					weight = organism.eval(cppn_inputs).at(0);

					if(abs(weight) > connection_threshold)
						(substrate->GetSpatialNode(i+1,k))->AddInputToNode(substrate->GetSpatialNode(i,j), weight);
				}
			}
		}		
	}	

	for(int i = 0; i < substrate->GetLayersNumber(); i++)
		for(int j = 0; j < substrate->GetLayerNodesNumber(i); j++)
			if((substrate->GetSpatialNode(i,j))->GetNodeType() == 2 && (substrate->GetSpatialNode(i,j))->ActiveNode())
			{
				OkConnections = true;
				return true; 
			}
				

	cerr << "ERROR: Does not exist any active output node" << endl;
	return false;
	
}

bool HyperNeat::EvaluateSubstrateConnections()
{
	if(substrate->GetLayersNumber() == 0){
		cout << "ERROR: Does not exist any substrate initialized" << endl;
		return false;
	}

	if(!OkConnections) 
	{
		cerr << "ERROR: Does not exist any active output node" << endl;
		return false;
	}

	for(int i = 0; i < substrate->GetLayersNumber(); i++)
		substrate->EvaluateSpatialNode(i);

	return true;
}

bool HyperNeat::HyperNeatFitness(double fitness, int organism_id)
{
	bool result = (cppn_neat->fitness_champion < fitness) ? true: false;

	if(result) {

		clog << endl << "\tNEW CHAMPION FITNESS\t-->\t" << fitness << endl;

		cppn_neat->fitness_champion = fitness;
		cppn_neat->champion = cppn_neat->organisms.at(organism_id);
	}

	cppn_neat->organisms.at(organism_id).fitness = fitness;

	return result;
}

void HyperNeat::HyperNeatEvolve()
{
	cppn_neat->print_to_file_currrent_generation();
	cppn_neat->epoch();
}

void HyperNeat::GetHyperNeatOutputFunctions(string plataform)
{
	CreateSubstrateConnections(-1);

	vector < string > OUTPUTS;
	vector < int > INPUT_ORDER;
	vector < int > OUTPUT_ORDER;

	OUTPUTS = substrate->GetSubstrateOutputFunctions(plataform);
	INPUT_ORDER = substrate->GetInputOrder();
	OUTPUT_ORDER = substrate->GetOutputOrder();

	if(!strcmp(plataform.c_str(),(char *)"octave"))
	{	
		stringstream file_name;
		file_name << "functions_files/" << test_name << ".m";
		ofstream myfile (file_name.str().c_str());

		substrate->GetNodeFunction(plataform);

		if (myfile.is_open()){

			myfile << "function [ ";
			for(int i = 0; i < (int)substrate->outputs.size(); i++){
				myfile << "OUTPUT_" << i ;
				if(i < (int)substrate->outputs.size()-1) myfile << ", ";
			}
			myfile << " ] = " << test_name << "( ";
			for(int i = 0; i < (int)substrate->inputs.size(); i++){
				myfile << "INPUT_" << i ;
				if(i < (int)substrate->inputs.size()-1) myfile << ", ";
			}
			myfile << " )" << endl;
			for(int i = 0; i < (int)substrate->outputs.size(); i++)
				myfile << OUTPUTS.at(i) << ";" << endl;
		    myfile.close();
	  	}else 
	  		cerr << "Unable to open file: " << file_name.str() << endl;
	}
	else if(!strcmp(plataform.c_str(),(char *)"c++"))
	{
		ofstream myfile ("functions_files/HYPERNEAT_FUNCTIONS.hpp");

		if (myfile.is_open()){

			myfile << "#ifndef HYPERNEAT_FUNCTIONS_H" << endl;
			myfile << "#define HYPERNEAT_FUNCTIONS_H" << endl << endl;
			myfile << "#include <math.h>" << endl;
			myfile << SIGMOID_CONST_STRING << endl;
			myfile << SIGMOID_STRING << endl;

			for(int i = 0; i < (int)substrate->outputs.size(); i++)
			{
				myfile << "#define " << test_name << "_" << OUTPUT_ORDER.at(i) << "(";

				for(int j = 0; j < (int)substrate->inputs.size(); j++)
				{
				 	myfile << "INPUT_" << j;
				 	if(j < (int)substrate->inputs.size()-1) myfile << ", ";
				 	else myfile << ") " << OUTPUTS.at(i) << endl;
				}
			}

			myfile << endl << "#endif" << endl;
		    myfile.close();
			
	  	}else 
	  		cerr << "Unable to open file: HYPERNEAT_FUNCTIONS" << endl;
	}
	
}

#endif