#include "PLExpression.h"
using namespace std;

namespace PL {
    ostream& operator<< (ostream& out, const Expression& e) {
        e.print(out);
        return out;
    }

    void Expression::accept(ExpressionVisitor* visitor) const {
        visitor->visit(*this);
    }

    void TrueExpression::print(ostream& out) const {
        out << "⊤";
    }
    void TrueExpression::accept(ExpressionVisitor* visitor) const {
        visitor->visit(*this);
    }
    bool TrueExpression::evaluate(const Context &) const {
        return true;
    }


    void FalseExpression::print(ostream& out) const {
        out << "⊥";
    }
    void FalseExpression::accept(ExpressionVisitor* visitor) const {
        visitor->visit(*this);
    }
    bool FalseExpression::evaluate(const Context &) const {
        return false;
    }

    void NotExpression::print(ostream& out) const {
        out << "¬" << *expr;
    }
    void NotExpression::accept(ExpressionVisitor* visitor) const {
        visitor->visit(*this);
    }
    bool NotExpression::evaluate(const Context& context) const {
        return !expr->evaluate(context);
    }


    void AndExpression::print(ostream& out) const {
        out << "(" << *lhs << " ∧ " << *rhs << ")";
    }
    void AndExpression::accept(ExpressionVisitor* visitor) const {
        visitor->visit(*this);
    }
    bool AndExpression::evaluate(const Context& context) const {
        return lhs->evaluate(context) && rhs->evaluate(context);
    }


    void OrExpression::print(ostream& out) const {
        out << "(" << *lhs << " ∨ " << *rhs << ")";
    }
    void OrExpression::accept(ExpressionVisitor* visitor) const {
        visitor->visit(*this);
    }
    bool OrExpression::evaluate(const Context& context) const {
        return lhs->evaluate(context) || rhs->evaluate(context);
    }


    void ImpliesExpression::print(ostream& out) const {
        out << "(" << *lhs << " → " << *rhs << ")";
    }
    void ImpliesExpression::accept(ExpressionVisitor* visitor) const {
        visitor->visit(*this);
    }
    bool ImpliesExpression::evaluate(const Context& context) const {
        return !lhs->evaluate(context) || rhs->evaluate(context);
    }


    void IffExpression::print(ostream& out) const {
        out << "(" << *lhs << " ↔ " << *rhs << ")";
    }
    void IffExpression::accept(ExpressionVisitor* visitor) const {
        visitor->visit(*this);
    }
    bool IffExpression::evaluate(const Context& context) const {
        return lhs->evaluate(context) == rhs->evaluate(context);
    }


    void VariableExpression::print(ostream& out) const {
        out << name;
    }
    void VariableExpression::accept(ExpressionVisitor* visitor) const {
        visitor->visit(*this);
    }
    bool VariableExpression::evaluate(const Context& context) const {
        return context.at(name);
    }


    void BowtieExpression::print(ostream& out) const {
        out << "⋈(" << *firstE << ", " << *secondE << ", " << *thirdE << ")";
    }
    void BowtieExpression::accept(ExpressionVisitor* visitor) const {
        visitor->visit(*this);
    }
    bool BowtieExpression::evaluate(const Context& context) const {
        /* Map from the subexpressions to a table row. We invert everything
         * to match the variable ordering from the midterm.
         */
        size_t value = 4 * !firstE->evaluate(context) +
                       2 * !secondE->evaluate(context) +
                       1 * !thirdE->evaluate(context);

        /* Sanity check. */
        if (value >= 8) throw logic_error("Internal error: Evaluated beyond array bounds. Contact the course staff.");

        /* Table from the handout. */
        const bool kTable[8] = {
            false,
            false,
            true,
            true,
            true,
            false,
            false,
            false
        };

        return kTable[value];
    }




    /* ExpressionTreeWalker implementation. */
    void ExpressionTreeWalker::visit(const Expression &) {
        throw runtime_error("Unknown expression type.");
    }
    void ExpressionTreeWalker::visit(const TrueExpression& e) {
        handle(e);
    }
    void ExpressionTreeWalker::visit(const FalseExpression& e) {
        handle(e);
    }
    void ExpressionTreeWalker::visit(const VariableExpression& e) {
        handle(e);
    }
    void ExpressionTreeWalker::visit(const NotExpression& e) {
        handle(e);
        e.underlying()->accept(this);
    }
    void ExpressionTreeWalker::visit(const AndExpression& e) {
        handle(e);
        e.left()->accept(this);
        e.right()->accept(this);
    }
    void ExpressionTreeWalker::visit(const OrExpression& e) {
        handle(e);
        e.left()->accept(this);
        e.right()->accept(this);
    }
    void ExpressionTreeWalker::visit(const ImpliesExpression& e) {
        handle(e);
        e.left()->accept(this);
        e.right()->accept(this);
    }
    void ExpressionTreeWalker::visit(const IffExpression& e) {
        handle(e);
        e.left()->accept(this);
        e.right()->accept(this);
    }
    void ExpressionTreeWalker::visit(const BowtieExpression& e) {
        handle(e);
        e.first()->accept(this);
        e.second()->accept(this);
        e.third()->accept(this);
    }

    /* Truth tables logic. */

    namespace {
        /* Given one variable assignment, advances to the next. If this isn't possible,
         * this returns false.
         */
        bool nextAssignment(vector<bool>& assignment) {
            /* Walk backwards to the next false slot. */
            for (size_t index = assignment.size(); index > 0; index--) {
                if (!assignment[index - 1]) {
                    assignment[index - 1] = true;
                    for (size_t i = index; i < assignment.size(); i++) {
                        assignment[i] = false;
                    }
                    return true;
                }
            }

            /* Oops, all true. */
            return false;
        }
    }

    vector<pair<vector<bool>, bool>> truthTableFor(Formula expr) {
        /* Begin by listing the variables in some predicable order. */
        vector<string> variables;
        for (const auto& var: variablesIn(expr)) {
            variables.push_back(var);
        }

        /* List of variables' values, from left to right. */
        vector<bool> curr(variables.size(), false);

        vector<pair<vector<bool>, bool>> result;

        do {
            /* Convert from list of booleans to evaluation context. */
            PL::Context context;
            for (size_t i = 0; i < curr.size(); i++) {
                context[variables[i]] = curr[i];
            }

            result.push_back(make_pair(curr, expr->evaluate(context)));
        } while (nextAssignment(curr));

        return result;
    }

    /* Returns all the variables in the given propositional logic formula. */
    set<string> variablesIn(shared_ptr<PL::Expression> expr) {
        class VariableWalker: public PL::ExpressionTreeWalker {
        public:
            set<string> result;

            void handle(const PL::VariableExpression& e) {
                result.insert(e.variableName());
            }
        };

        VariableWalker walker;
        expr->accept(&walker);
        return walker.result;
    }
}
