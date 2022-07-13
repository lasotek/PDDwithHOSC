#ifndef __INTERPRETER_H__
#define __INTERPRETER_H__
#include <regex>
#include <unordered_map>
#include <memory>
#include <stack>
#include <exception>
#include <set>
#include <algorithm>
#include <cctype>
#include "../Cluster.h"

namespace prs
{

    inline std::string to_lower(const std::string &str)
    {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }

    class format_error : public std::logic_error
    {
    public:
        using std::logic_error::logic_error;
    };

    inline bool is_float(const std::string &str)
    {
        static std::regex float_test("^[\\+|-]?([0-9]*[.])?[0-9]+([e|E][\\+|-]?[0-9]+)?$");
        return std::regex_match(str, float_test);
    }

    inline bool is_int(const std::string &str)
    {
        static std::regex int_test("^[\\+|-]?[0-9]+$");
        return std::regex_match(str, int_test);
    }

    inline bool is_unsigned(const std::string &str)
    {
        static std::regex int_test("^\\+?[0-9]+$");
        return std::regex_match(str, int_test);
    }
    inline bool is_name(const std::string &str)
    {
        static std::regex int_test("^[a-z,A-Z,_][a-z,A-Z,0-9,_]*$");
        return std::regex_match(str, int_test);
    }

    using Shared_cluster = std::shared_ptr<HOSC::KICluster>;
    using Cluster_map = std::unordered_map<std::string, Shared_cluster>;

    class Interpreter;
    class OperationContainer
    {
    protected:
        Interpreter &interpreter_;

        Cluster_map &cluster_map();
        bool cluster_exists(const std::string &cluster_name);
        virtual Shared_cluster get_cluster_if_exists(const std::string &cluster_name);
        void push_operation(std::shared_ptr<OperationContainer> operation);

    public:
        OperationContainer(Interpreter &interpreter) : interpreter_(interpreter) {}
        virtual bool make_operation(const std::string &str) = 0;
        virtual void next_operation_finished(OperationContainer &next_operator)  {}
    };

    using OperationStack = std::stack<std::shared_ptr<OperationContainer>>;

    class DefCluster : public OperationContainer
    {
    private:
        enum class Expectations
        {
            none,
            cluster_definition_expected,
            cluster_usage_expected,
            cluster_force_operation_expected,
            cluster_removal
        };
        Expectations expectations = Expectations::none;
        Shared_cluster cluster_ptr = nullptr;

    public:
        using OperationContainer::OperationContainer;
        virtual bool make_operation(const std::string &str) override;
        virtual void next_operation_finished(OperationContainer &next_operator) override;//must be
    };

    class InterfaceDef : public OperationContainer
    {
    private:
        friend class DefCluster;
        std::string cluster_name_;

        std::set<int> interface_pins_;
        bool left_bracket_expected_ = true;
        bool comma_expected_ = false;
        bool first_param = true;

    public:
        using OperationContainer::OperationContainer;
        InterfaceDef(Interpreter &interpreter, std::string cluster_name) : OperationContainer(interpreter), cluster_name_(cluster_name) {}
        virtual bool make_operation(const std::string &str) override;
        // virtual void next_operation_finished(OperationContainer &next_operator) override;
    };

    class ClusterInteriorDef : public OperationContainer
    {
    private:
        Shared_cluster cluster_ptr_;
        bool left_bracket_expected_ = true;
        std::string &cluster_name_;

    protected:
        virtual Shared_cluster get_cluster_if_exists(const std::string &cluster_name);

    public:
        using OperationContainer::OperationContainer;
        ClusterInteriorDef(Interpreter &interpreter, Shared_cluster cluster_ptr, std::string cluster_name) : OperationContainer(interpreter), cluster_ptr_(cluster_ptr), cluster_name_(cluster_name) {}
        virtual bool make_operation(const std::string &str) override;
        virtual void next_operation_finished(OperationContainer &next_operator) override;
    };

    class EdgeDef : public OperationContainer
    {
    private:
        friend class ClusterInteriorDef;
        Shared_cluster cluster_ptr_;
        std::string edge_name_;
        bool left_bracket_expected_ = true;
        int node_a_ = -1;
        int node_b_ = -1;
        long long weight_ = 1;
        int value_counter_ = 0;
        bool comma_expected_ = false;

    public:
        using OperationContainer::OperationContainer;
        EdgeDef(Interpreter &interpreter, Shared_cluster cluster_ptr) : OperationContainer(interpreter), cluster_ptr_(cluster_ptr) {}
        virtual bool make_operation(const std::string &str) override;
        // virtual void next_operation_finished(OperationContainer &next_operator) override;
    };

    class ClusterInserter : public OperationContainer
    {
    private:
        friend class ClusterInteriorDef;
        Shared_cluster p_cluster_ = nullptr;
        HOSC::KICluster::set_of_nodes nodes_;
        HOSC::KICluster::boundary_connections_list boundary_connections_;
        bool left_bracket_expected_ = true;
        bool comma_expected_ = false;
        bool left_node_expected_ = false;
        bool right_node_expected_ = false;
        bool arrow_expected_1_ = false;
        bool arrow_expected_2_ = false;
        unsigned left_node_ = 0;
        unsigned right_node_ = 0;
    public:
        using OperationContainer::OperationContainer;
        ClusterInserter(Interpreter &interpreter, Shared_cluster cluster) : OperationContainer(interpreter), p_cluster_(cluster) {
            nodes_ = p_cluster_->get_bound_nodes_ext();
        }
        virtual bool make_operation(const std::string &str) override;
        // virtual void next_operation_finished(OperationContainer &next_operator) override;
    };

    class EdgeRemover : public OperationContainer
    {
        Shared_cluster p_cluster_ = nullptr;
        bool cluster_name_expected_ = true;
    public:
        using OperationContainer::OperationContainer;
        virtual bool make_operation(const std::string &str) override;
        // virtual void next_operation_finished(OperationContainer &next_operator) override;
    };
    
    class EdgeModifier : public OperationContainer
    {
    public:
        using OperationContainer::OperationContainer;
        virtual bool make_operation(const std::string &str) override;
        // virtual void next_operation_finished(OperationContainer &next_operator) override;
    };
    
    class EdgeInserter : public OperationContainer
    {
    public:
        using OperationContainer::OperationContainer;
        virtual bool make_operation(const std::string &str) override;
        // virtual void next_operation_finished(OperationContainer &next_operator) override;
    };
    
    class ClusterSolver : public OperationContainer
    {
    public:
        using OperationContainer::OperationContainer;
        virtual bool make_operation(const std::string &str) override;
        // virtual void next_operation_finished(OperationContainer &next_operator) override;
    };

    class ClusterPrinter : public OperationContainer
    {
    public:
        using OperationContainer::OperationContainer;
        virtual bool make_operation(const std::string &str) override;
        // virtual void next_operation_finished(OperationContainer &next_operator) override;
    };

    class Interpreter
    {
    private:
        friend class OperationContainer;
        Cluster_map cluster_map_;
        OperationStack operation_stack_;
        enum class statement_type
        {
            none,
            cluster,
            cluster_param,
            cluster_param_value,
            cluster_param_value_float,
            cluster_param_value_int,
        };
        statement_type current_statement_type_ = statement_type::none;

        inline bool cluster_exists(const std::string &cluster_name)
        {
            return cluster_map_.find(cluster_name) != cluster_map_.end();
        }

        /**
         * @brief Get the cluster if exists object
         *
         * @param cluster_name cluster name
         * @return Shared_cluster nullptr if not exists, otherwise the cluster object shared pointer
         */
        Shared_cluster get_cluster_if_exists(const std::string &cluster_name);

    public:
        Interpreter();
        ~Interpreter();

        /**
         * @brief push a statement to the interpreter
         *
         * @param statement string statement
         * @return true accepted
         * @return false rejected (invalid statement)
         */
        bool push_statement(const std::string &statement);

        /**
         * @brief reset the interpreter
         *
         */
        inline void reset();
    };

    inline Cluster_map &OperationContainer::cluster_map()
    {
        return interpreter_.cluster_map_;
    }

    inline bool OperationContainer::cluster_exists(const std::string &cluster_name)
    {
        return interpreter_.cluster_exists(cluster_name);
    }
    inline Shared_cluster OperationContainer::get_cluster_if_exists(const std::string &cluster_name)
    {
        return interpreter_.get_cluster_if_exists(cluster_name);
    }

    inline void OperationContainer::push_operation(std::shared_ptr<OperationContainer> operation)
    {
        interpreter_.operation_stack_.push(operation);
    }
} // namespace prs

#endif // __INTERPRETER_H__