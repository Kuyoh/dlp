#ifndef DLP_PARSER_ASTNODE_HPP
#define DLP_PARSER_ASTNODE_HPP

#include <memory>
#include <string>
#include <list>
#include "AstVisitor.hpp"

namespace dlp {
	namespace ast {
		template <typename T> using UPtrList = std::list<std::unique_ptr<T>>;

		struct INode {
			virtual ~INode() {};
			virtual void visit(Visitor &v) = 0;
			
			struct Location {
				std::string *fileName;
				unsigned int lineStart, lineEnd;
				unsigned int charStart, charEnd;
			} location;
		};

		struct IExpression : INode {};
		struct IStatement : INode {};
		struct IType : INode { };
		struct IBlock : IStatement {};
	}
}
#endif
