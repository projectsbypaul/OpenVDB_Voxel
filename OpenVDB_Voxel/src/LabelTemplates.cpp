#include "../include/LabelTemplates.h"

namespace LabelTemplates {
	//Costum Templates

	LabelTemplate get_template_abc_inside_outside() {

		std::vector<std::string> class_list{
			"BSpline",
			"Cone",
			"Cylinder",
			"Plane",
			"Sphere",
			"Torus",
			"Inside",
			"Outside"
		};

		return LabelTemplate(class_list);
	}

	LabelTemplate get_template_abc_edge() {

		std::vector<std::string> class_list{
			"BSpline",
			"Cone",
			"Cylinder",
			"Plane",
			"Sphere",
			"Torus",
			"Edge",
			"Inside",
			"Outside"
		};

		return LabelTemplate(class_list);
	}

	LabelTemplate get_template_abc_full() {

		std::vector<std::string> class_list{
			"BSpline",
			"Cone",
			"Cylinder",
			"Extrusion",
			"Other",
			"Plane",
			"Revolution",
			"Sphere",
			"Torus",
			"Inside",
			"Outside"
		};

		return LabelTemplate(class_list);
	}

	LabelTemplate get_template_abc_full_edge() {

		std::vector<std::string> class_list{
			"BSpline",
			"Cone",
			"Cylinder",
			"Extrusion",
			"Other",
			"Plane",
			"Revolution",
			"Sphere",
			"Torus",
			"Edge",
			"Inside",
			"Outside"
		};

		return LabelTemplate(class_list);
	}

	//acessability functions 
	LabelTemplate get_template_from_string(const std::string& requested_template) {

		if (requested_template == "inside_outside") {
			return get_template_abc_inside_outside();
		}
		else if (requested_template == "edge") {
			return get_template_abc_edge();
		}
		else if (requested_template == "full") {
			return get_template_abc_full();
		}
		else if (requested_template == "full_edge") {
			return get_template_abc_full_edge();
		}
		else {
			throw std::invalid_argument("Unknown template: " + requested_template);
		}
	}
}