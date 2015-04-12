#ifndef SPATIALNODE_CPP
#define SPATIALNODE_CPP

#include "SpatialNode.hpp"

using namespace ANN_USM;

SpatialNode::SpatialNode(int node_type, int sheet_id, vector < double > coordenates, char * node_function)
{
	this->node_type = node_type;
	this->sheet_id = sheet_id;
	this->coordenates = coordenates;

	int size = strlen(node_function)+1;
	this->node_function = new char[size];
	strncpy(this->node_function, node_function, size);

	if(!strcmp(node_function,(char *)"SIGMOID"))
	{
		NodeFunction = &SpatialNode::Sigmoid;		
	}
	/*
	else if(!strcmp(node_function,(char *)"USER_CUSTOM"))
	{
		NodeFunction = &SpatialNode::User_custom;
	}
	*/
	else
	{
		cerr << "ERROR: Does not exist any function called " << node_function << endl;
	}

	output = new double(0.0);

	if(node_type == 0) active = true;
	else active = false;
}

SpatialNode::SpatialNode()
{
	
}

SpatialNode::~SpatialNode()
{
	vector < double >().swap(coordenates);
	vector < double >().swap(inputs_weight);
	vector < SpatialNode * >().swap(inputs_nodes);
}

void SpatialNode::SetInputToInputNode(double * input, int input_id)
{
	if (node_type != 0)
	{
		cerr << "ERROR: This node is not of type input" << endl;
		return;
	}

	this->input = input;
	this->input_id = input_id;
}

void SpatialNode::SetOutputToOutputNode(double * output, int output_id)
{
	if (node_type != 2)
	{
		cerr << "ERROR: This node is not of type output" << endl;
		return;
	}

	this->output = output;
	this->output_id = output_id;
}

void SpatialNode::AddInputToNode(SpatialNode * input_node, double input_weight)
{
	if(node_type == 0)
	{
		cerr << "ERROR: Can not connect to a node of type input" << endl;
		return;
	}

	if (input_node->GetSheetNodeId() >= sheet_id)
	{			
		cerr << "ERROR: Can not make a recurrent connection" << endl;
		return;
	}

	if(!active) active = input_node->ActiveNode();

	inputs_nodes.push_back(input_node);
	inputs_weight.push_back(input_weight);
}

void SpatialNode::OutputCalcule()
{
	double aux = (node_type == 0) ? *input : 0.0;

	for(int i = 0; i < (int)inputs_nodes.size(); i++)
	{
		aux += (inputs_nodes.at(i)->GetOuput())*inputs_weight.at(i);
	}

	*output = (this->*NodeFunction)(aux);
}

vector < double > SpatialNode::GetCoordenates()
{
	return coordenates;
}

int SpatialNode::GetNodeType()
{
	return node_type;
}

int SpatialNode::GetSheetNodeId()
{
	return sheet_id;
}

double SpatialNode::GetOuput()
{
	return *output;
}

void SpatialNode::ClearInputs()
{	
	inputs_nodes.clear();
	inputs_weight.clear();
	if(node_type != 0) active = false;
}

string SpatialNode::GetOutputNodeFunction(string plataform)
{
	stringstream function;

	if(!strcmp(plataform.c_str(),(char *)"octave"))
	{
		if(node_type == 2) function << "OUTPUT_" << output_id << " = ";

		function << node_function << "( ";

		if(node_type == 0) function << "INPUT_" << input_id;		
		else
		{
			if((int)inputs_nodes.size() > 0)
				for(int i = 0; i < (int)inputs_nodes.size(); i++)
				{
					function << inputs_nodes.at(i)->GetOutputNodeFunction(plataform) << "* " << inputs_weight.at(i);
					if( i + 1 < (int)inputs_nodes.size() ) function << " + ";
				}
			else
				function << "0";
		}
			
		function << " ) ";
	}
	else if(!strcmp(plataform.c_str(),(char *)"c++"))
	{		
		function << node_function << "( ";

		if(node_type == 0) function << "INPUT_" << input_id;		
		else
		{
			if((int)inputs_nodes.size() > 0)
				for(int i = 0; i < (int)inputs_nodes.size(); i++)
				{
					function << inputs_nodes.at(i)->GetOutputNodeFunction(plataform) << "* " << inputs_weight.at(i);
					if( i + 1 < (int)inputs_nodes.size() ) function << " + ";
				}
			else
				function << "0";
		}
			
		function << " ) ";
	}

	return function.str();
}

int SpatialNode::GetInputId()
{
	return input_id;
}

int SpatialNode::GetOutputId()
{
	return output_id;
}

bool SpatialNode::ActiveNode()
{
	return active;
}

double SpatialNode::Sigmoid(double input)
{
	return SIGMOID(input);
}

#endif