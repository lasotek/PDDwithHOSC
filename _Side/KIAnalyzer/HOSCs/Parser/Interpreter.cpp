#include "Interpreter.h"

namespace prs
{
    Interpreter::Interpreter(/* args */)
    {
        // DefCluster def_cluster(*this);
        auto def_cluster_ptr = std::make_shared<DefCluster>(*this);
        operation_stack_.push(def_cluster_ptr);
    }

    inline void Interpreter::reset()
    {
        cluster_map_.clear();
        auto def_cluster_ptr = std::make_shared<DefCluster>(*this);
        operation_stack_.push(def_cluster_ptr);
        current_statement_type_ = statement_type::none;
    }

    Interpreter::~Interpreter()
    {
    }

    Shared_cluster Interpreter::get_cluster_if_exists(const std::string &cluster_name)
    {
        auto it = cluster_map_.find(cluster_name);

        return it != cluster_map_.end() ? it->second : nullptr;
    }

    bool Interpreter::push_statement(const std::string &statement)
    {
        if (!operation_stack_.top()->make_operation(statement))
        {
            auto top = operation_stack_.top();
            operation_stack_.pop();
            operation_stack_.top()->next_operation_finished(*top);
        }
        // auto is_name1 = is_name(statement);
        // if (is_name1)
        //     std::cout << "# the " << statement << " statement is literal: " << std::endl;
        // else
        //     std::cout << "# statement: " << statement << " is not a literal" << std::endl;
        return true;
    }

    bool DefCluster::make_operation(const std::string &str)
    {
        {
            auto lower_str = to_lower(str);
            if (lower_str == "delete")
            {
                push_operation(std::make_shared<EdgeRemover>(interpreter_));
                return true;
            }
            if (lower_str == "print")
            {
                push_operation(std::make_shared<ClusterPrinter>(interpreter_));
                return true;
            }
            if (lower_str == "solve")
            {
                push_operation(std::make_shared<ClusterSolver>(interpreter_));
                return true;
            }
            if (lower_str == "add")
            {
                push_operation(std::make_shared<EdgeInserter>(interpreter_));
                return true;
            }
            if (lower_str == "modify")
            {
                push_operation(std::make_shared<EdgeModifier>(interpreter_));
                return true;
            }
        }

        if (is_name(str))
        {
            if (cluster_exists(str))
            {
                expectations = Expectations::cluster_usage_expected;
                auto cluster_ptr = get_cluster_if_exists(str);
                if (cluster_ptr)
                {
                    push_operation(std::make_shared<ClusterInserter>(interpreter_, cluster_ptr));
                }
                else
                {
                    std::string msg = "Error:  Impossible! " + str + " is not a cluster name";
                    throw format_error(msg.c_str());
                }
            }
            else
            {
                expectations = Expectations::cluster_definition_expected;
                push_operation(std::make_shared<InterfaceDef>(interpreter_, str));
            }
            return true;
        }
        else
        {
            std::string msg = "Error: " + str + " is not a legal name";
            throw format_error(msg.c_str());
        }
    }

    void DefCluster::next_operation_finished(OperationContainer &next_operator)
    {
        if (typeid(next_operator) == typeid(InterfaceDef))
        {
            auto &interface_def = *dynamic_cast<InterfaceDef *>(&next_operator);
            auto &list = interface_def.interface_pins_;
            auto &map = cluster_map();
            cluster_ptr = map[interface_def.cluster_name_] = std::make_shared<HOSC::KICluster>(list, true);
            push_operation(std::make_shared<ClusterInteriorDef>(interpreter_, cluster_ptr, interface_def.cluster_name_));
        }
        // else if (typeid(next_operator) == typeid(ClusterInteriorDef))
        // {
        // }
        // else
        // {
        //     std::string msg = "Error:  is not a legal statement";
        //     throw format_error(msg.c_str());
        // }
    }

    bool InterfaceDef::make_operation(const std::string &str)
    {
        if (left_bracket_expected_)
        {
            if (str != "(")
                throw format_error("Error:  expected (");
            left_bracket_expected_ = false;
            return true;
        }

        if (str == ")")
            if (comma_expected_ || first_param)
                return false;
            else
                throw format_error("Error:  You are not allowed to use ) after ,");
        if (comma_expected_)
            if (str != ",")
                throw format_error("Error:  expected \",\" or \")\"");
            else
            {
                comma_expected_ = false;
                return true;
            }
        if (!is_unsigned(str))
        {
            std::string msg = "Error:  Any node must be an unsigned integer number! " + str + " is not a legal integer";
            throw format_error(msg.c_str());
        }
        auto node_id = std::stoi(str);
        if (interface_pins_.contains(node_id))
        {
            std::string msg = "Error:  Node " + str + " is already in the interface";
            throw format_error(msg.c_str());
        }
        interface_pins_.insert(node_id);
        comma_expected_ = true;
        first_param = false;
        return true;
    }
    // void InterfaceDef::next_operation_finished(OperationContainer &next_operator)
    // {
    // }

    bool ClusterInserter::make_operation(const std::string &str)
    {
        if (left_bracket_expected_)
        {
            if (str != "(")
                throw format_error("Error:  expected (");
            left_bracket_expected_ = false;
            left_node_expected_ = true;
            return true;
        }
        if (str == ")")
            if (comma_expected_)
                return false;
            else
                throw format_error("Error:  You are not allowed to use ) in this place");
        if (comma_expected_)
        {
            if (str != ",")
                throw format_error("Error:  expected \",\" or \")\"");
            else
            {
                comma_expected_ = false;
                left_node_expected_ = true;
                return true;
            }
        }
        if (arrow_expected_1_)
        {
            if (str != "=")
                throw format_error("Error:  expected \"=>\"");
            arrow_expected_1_ = false;
            arrow_expected_2_ = true;
            return true;
        }
        if (arrow_expected_2_)
        {
            if (str != ">")
                throw format_error("Error:  expected \"=>\"");
            arrow_expected_2_ = false;
            right_node_expected_ = true;
            return true;
        }
        if (left_node_expected_)
        {
            if (!is_unsigned(str))
                throw format_error("Error:  Any node must be an unsigned integer number! " + str + " is not a legal integer");
            left_node_ = std::stoi(str);
            if (!nodes_.contains(left_node_))
                throw format_error("Error:  Node " + str +
                                   ". The node must be interface node of the cluster and can be used only once.");
            nodes_.erase(left_node_);
            left_node_expected_ = false;
            arrow_expected_1_ = true;
            return true;
        }
        if (right_node_expected_)
        {
            if (!is_unsigned(str))
                throw format_error("Error:  Any node must be an unsigned integer number! " + str + " is not a legal integer");
            right_node_ = std::stoi(str);
            boundary_connections_.push_back(std::make_pair(left_node_, right_node_));
            right_node_expected_ = false;
            comma_expected_ = true;
            return true;
        }
        return true;
    }
    // void ClusterInserter::next_operation_finished(OperationContainer &next_operator)
    // {
    // }

    bool ClusterInteriorDef::make_operation(const std::string &str)
    {
        if (left_bracket_expected_)
        {
            if (str != "{")
                throw format_error("Error:  expected (");
            left_bracket_expected_ = false;
            return true;
        }
        if (str == "}")
            return false;
        if (str == "e" || str == "E")
        {
            // expectations = Expectations::cluster_usage_expected;
            push_operation(std::make_shared<EdgeDef>(interpreter_, cluster_ptr_));
            return true;
        }
        else if (is_name(str))
        {
            // expectations = Expectations::cluster_usage_expected;
            auto cluster_ptr = get_cluster_if_exists(str);
            if (cluster_ptr)
            {
                push_operation(std::make_shared<ClusterInserter>(interpreter_, cluster_ptr));
            }
            else
            {
                std::string msg = "Error:  Impossible! " + str + " is not a defined cluster name";
                throw format_error(msg.c_str());
            }
            return true;
        }
        else
        {
            std::string msg = "Error:  " + str + " is not a legal cluster name or edge";
            throw format_error(msg.c_str());
        }
        return true;
    }
    void ClusterInteriorDef::next_operation_finished(OperationContainer &next_operator)
    {
        if (typeid(next_operator) == typeid(EdgeDef))
        {
            auto &edge_def = *dynamic_cast<EdgeDef *>(&next_operator);
            cluster_ptr_->AddEdge({edge_def.node_a_, edge_def.node_b_, edge_def.weight_, edge_def.edge_name_});
            // auto &list = edge_def.edge_pins_;
            // cluster_ptr_->add_edge(list);
        }
        else if (typeid(next_operator) == typeid(ClusterInserter))
        {
            auto &cluster_ins = *dynamic_cast<ClusterInserter *>(&next_operator);
            cluster_ptr_->insert_cluster(cluster_ins.p_cluster_, cluster_ins.boundary_connections_);
        }
        else
        {
            std::string msg = "Error:  is not a legal statement";
            throw format_error(msg.c_str());
        }
    }

    Shared_cluster ClusterInteriorDef::get_cluster_if_exists(const std::string &cluster_name)
    {
        if (cluster_name == cluster_name_)
        {
            std::string msg = "Error:  " + cluster_name + " - you cannot use itself ";
            throw format_error(msg.c_str());
        }
        return OperationContainer::get_cluster_if_exists(cluster_name);
    }

    bool EdgeDef::make_operation(const std::string &str)
    {
        if (left_bracket_expected_)
        {
            if (str != "(")
            {
                if (edge_name_.empty())
                    if (is_name(str))
                    {
                        edge_name_ = str;
                        return true;
                    }
                    else
                    {
                        throw format_error("Error:  expected ( or a name of the edge");
                    }

                throw format_error("Error:  expected (");
            }
            left_bracket_expected_ = false;
            return true;
        }
        else
        {
            if (str == ")")
                if (value_counter_ == 2 || value_counter_ == 3)
                    return false;
                else
                    throw format_error("Error:  node_a, node_b and optional weight only expected. ) was expected");
            if (value_counter_ > 3)
                throw format_error("Error:  ) was expected. Too many parameters.");
            if (comma_expected_)
            {
                if (str != ",")
                    throw format_error("Error:  expected \",\" or \")\"");
                else
                {
                    comma_expected_ = false;
                    return true;
                }
            }
            else if (is_int(str))
            {
                auto node_id = std::stoi(str);
                if ((value_counter_ == 0 || value_counter_ == 1) && node_id < 0)
                    throw format_error("Error:  node id must be positive");
                if (value_counter_ == 0)
                    node_a_ = node_id;
                else if (value_counter_ == 1)
                    node_b_ = node_id;
                else if (value_counter_ == 2)
                    weight_ = node_id;
                else
                    throw format_error("Error:  node_a, node_b and optional weight only expected");
                value_counter_++;
                comma_expected_ = true;
                return true;
            }
            else
            {
                std::string msg = "Error:  " + str + " is not an integer number";
                throw format_error(msg.c_str());
            }
        }
        return true;
    }
    // void EdgeDef::next_operation_finished(OperationContainer &next_operator)
    // {
    // }

    bool EdgeRemover::make_operation(const std::string &str)
    {
        if (cluster_name_expected_)
        {
            p_cluster_ = get_cluster_if_exists(str);
            if (!p_cluster_)
            {
                throw format_error("Error:  " + str + " is not a defined cluster name");
            }
            cluster_name_expected_ = false;
        } else
        {
            if(!p_cluster_->IsEdge(str))
            {
                throw format_error("Error:  " + str + " is not a defined edge name");
            }
            else 
            {
                p_cluster_->DeleteEdge(str);
                return true;
            }
        }
        // interpreter_.
        return true;
    }

    bool EdgeModifier::make_operation(const std::string &str)
    {
        return true;
    }

    bool EdgeInserter::make_operation(const std::string &str)
    {
        return true;
    }

    bool ClusterSolver::make_operation(const std::string &str)
    {
        auto p_cluster_ = get_cluster_if_exists(str);
        if (!p_cluster_)
        {
            throw format_error("Error:  " + str + " is not a defined cluster name");
        }
        p_cluster_->Solve2();
        return false;
    }

    bool ClusterPrinter::make_operation(const std::string &str)
    {
        auto p_cluster_ = get_cluster_if_exists(str);
        if (!p_cluster_)
        {
            throw format_error("Error:  " + str + " is not a defined cluster name");
        }
        p_cluster_->Solve2();
        std::cout<<*p_cluster_<<std::endl;
        return false;
    }

} // namespace spr
