#pragma once

#include <string>
#include <vector>

namespace e2ml {
	enum class IOType {Text, Bin};

	template<typename T>
	class Value {
		void *data;
	};

	class Node {
	public:
		std::string name;
		std::string path;
		std::vector<Node> children;
	};

	class Data {
	private:
		Node root;

		void parse();

	public:
		void load(const std::string &fileName, IOType rt = IOType::Text);
		void save(const std::string &fileName, IOType rt = IOType::Text);

		template<typename T>
		Value<T> *get_as(const std::string &key) {
			return nullptr;
		}

		inline std::vector<Node>::iterator begin() {
			return root.children.begin();
		}

		inline std::vector<Node>::iterator end() {
			return root.children.end();
		}

		inline Node operator*() {
			return *root.children.data();
		}
	};
}
