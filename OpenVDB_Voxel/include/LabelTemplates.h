#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace LabelTemplates{

	struct LabelTemplate
	{
		
		std::vector<std::string> label_class_list;
		std::unordered_map<std::string, int> class_to_index;
		std::unordered_map<int, std::string> index_to_class;
		int class_count;

		//construc empty 
		LabelTemplate() = default;

		//construc with list
		explicit LabelTemplate(std::vector<std::string> class_list) {
			class_count = class_list.size();
			int idx = 0;
			for (std::string label_class : class_list) {

				class_to_index.emplace(label_class, idx);
				index_to_class.emplace(idx, label_class);

				idx++;
			}
		}

		//getters
		std::unordered_map<std::string, int> get_class_to_index() {return class_to_index;}
		std::unordered_map<int, std::string> get_index_to_class() { return index_to_class;}

		//conversions
		int to_index(std::string label) {return class_to_index.at(label);}
		std::string to_label(int index) {return index_to_class.at(index); }
	};
	//CostumTemplates
	LabelTemplate get_template_abc_inside_outside();
	LabelTemplate get_template_abc_edge();
	LabelTemplate get_template_abc_full();
	LabelTemplate get_template_abc_full_edge();
	LabelTemplate get_template_from_string(const std::string& requested_template);
	//Accessability
}