#include <memory>
#include <rclcpp/rclcpp.hpp>
#include <interactive_markers/interactive_marker_server.hpp>
#include <visualization_msgs/msg/interactive_marker.hpp>
#include <visualization_msgs/msg/interactive_marker_control.hpp>
#include <visualization_msgs/msg/marker.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>

using namespace std::placeholders;
using visualization_msgs::msg::InteractiveMarker;
using visualization_msgs::msg::InteractiveMarkerControl;
using visualization_msgs::msg::InteractiveMarkerFeedback;
using visualization_msgs::msg::Marker;

class EEMarker : public rclcpp::Node {
    public:
        EEMarker() : Node("ee_marker_node") {
            // server_ = std::make_shared<interactive_markers::InteractiveMarkerServer>("ee_marker_server", this);
            server_ = std::make_shared<interactive_markers::InteractiveMarkerServer>(
                "ee_marker_server",
                this->get_node_base_interface(),
                this->get_node_clock_interface(),
                this->get_node_logging_interface(),
                this->get_node_topics_interface(),
                this->get_node_services_interface()
            );

            pose_pub_ = this->create_publisher<geometry_msgs::msg::PoseStamped>("/ee_goal", 10);

            makeMarker();

            // Replace the timer lambda with this:
            // republish_timer_ = this->create_wall_timer(
            //     std::chrono::seconds(1),
            //     [this]() {
            //         // Check subscriber count on the update topic directly
            //         auto topic_names_and_types = this->get_topic_names_and_types();
            //         std::string update_topic = "/ee_marker_server/update";

            //         size_t sub_count = this->count_subscribers(update_topic);
            //         if (sub_count > 0) {
            //         republish_timer_->cancel();
            //         RCLCPP_INFO(this->get_logger(), "RViz2 connected, stopping republish timer");
            //         return;
            //         }
            //         server_->applyChanges();
            //         RCLCPP_DEBUG(this->get_logger(), "Republishing marker (waiting for RViz2)...");
            //     }
            // );

            init_timer_ = this->create_wall_timer(
                std::chrono::milliseconds(500),
                [this]() {
                    if (this->count_subscribers("/ee_marker_server/update") > 0) {
                        init_timer_->cancel();
                        server_->applyChanges();
                        RCLCPP_INFO(this->get_logger(), "RViz2 connected, marker sent");
                    }
                }
            );

            RCLCPP_INFO(this->get_logger(), "Marker Node Ready");
        }

    private:
        std::shared_ptr<interactive_markers::InteractiveMarkerServer> server_;
        rclcpp::Publisher<geometry_msgs::msg::PoseStamped>::SharedPtr pose_pub_;
        // rclcpp::TimerBase::SharedPtr republish_timer_;
        rclcpp::TimerBase::SharedPtr init_timer_;


        static InteractiveMarkerControl makeAxisControl(const std::string & name, double qw, double qx, double qy, double qz, uint8_t mode) {
            InteractiveMarkerControl ctrl;
            ctrl.name = name;
            ctrl.orientation.w = qw;
            ctrl.orientation.x = qx;
            ctrl.orientation.y = qy;
            ctrl.orientation.z = qz;
            ctrl.interaction_mode = mode;
            return ctrl;
        }

        // void makeMarker() {
        //     visualization_msgs::msg::InteractiveMarker int_marker;
        //     int_marker.header.frame_id = "base_link";
        //     int_marker.name = "ee_control";
        //     int_marker.description = "end effector goal";
        //     int_marker.scale = 0.2;

        //     int_marker.pose.position.x = 0.3;
        //     int_marker.pose.position.y = 0.0;
        //     int_marker.pose.position.z = 0.3;
        //     int_marker.pose.orientation.w = 1.0;  // REQUIRED
            
        //     visualization_msgs::msg::InteractiveMarkerControl control;
        //     control.interaction_mode = visualization_msgs::msg::InteractiveMarkerControl::MOVE_ROTATE_3D;
            
        //     visualization_msgs::msg::Marker box_marker;
        //     box_marker.type = visualization_msgs::msg::Marker::SPHERE;
        //     box_marker.scale.x = 0.2;
        //     box_marker.scale.y = 0.2;
        //     box_marker.scale.z = 0.2;
        //     box_marker.color.r = 0.5;
        //     box_marker.color.g = 0.5;
        //     box_marker.color.b = 0.5;
        //     box_marker.color.a = 1.0;
            
            
        //     control.markers.push_back(box_marker);
        //     control.always_visible = true;
            
        //     int_marker.controls.push_back(control);


        //     RCLCPP_INFO(this->get_logger(), "marker visible");

        //     server_->insert(int_marker, std::bind(&EEMarker::processFeedback, this, _1));

        //     server_->applyChanges();
        // }


        void makeMarker() {
            InteractiveMarker int_marker;
            int_marker.header.frame_id = "base_link";
            int_marker.name = "ee_control";
            int_marker.description = "end effector goal";
            int_marker.scale = 0.2;

            int_marker.pose.position.x = 0.3;
            int_marker.pose.position.y = 0.0;
            int_marker.pose.position.z = 0.3;
            int_marker.pose.orientation.w = 1.0;

            Marker sphere;
            sphere.type = Marker::SPHERE;
            sphere.scale.x = sphere.scale.y = sphere.scale.z = 0.2;
            sphere.color.r = 0.5f;
            sphere.color.g = 0.0f;
            sphere.color.b = 0.8f;
            sphere.color.a = 1.0f;

            InteractiveMarkerControl visual;
            visual.always_visible = true;
            visual.interaction_mode = InteractiveMarkerControl::NONE;
            visual.markers.push_back(sphere);
            int_marker.controls.push_back(visual);

            // X axis
            int_marker.controls.push_back(makeAxisControl("rotate_x", 0.707, 0.707, 0.0, 0.0, InteractiveMarkerControl::ROTATE_AXIS));
            int_marker.controls.push_back(makeAxisControl("move_x",   0.707, 0.707, 0.0, 0.0, InteractiveMarkerControl::MOVE_AXIS));

            // Z axis
            int_marker.controls.push_back(makeAxisControl("rotate_z", 0.707, 0.0, 0.707, 0.0, InteractiveMarkerControl::ROTATE_AXIS));
            int_marker.controls.push_back(makeAxisControl("move_z",   0.707, 0.0, 0.707, 0.0, InteractiveMarkerControl::MOVE_AXIS));

            // Y axis
            int_marker.controls.push_back(makeAxisControl("rotate_y", 0.707, 0.0, 0.0, 0.707, InteractiveMarkerControl::ROTATE_AXIS));
            int_marker.controls.push_back(makeAxisControl("move_y",   0.707, 0.0, 0.0, 0.707, InteractiveMarkerControl::MOVE_AXIS));

            
            server_->insert(int_marker, std::bind(&EEMarker::processFeedback, this, _1));
            
            server_->applyChanges();
            RCLCPP_INFO(this->get_logger(), "marker created");
        }

        void processFeedback(const visualization_msgs::msg::InteractiveMarkerFeedback::ConstSharedPtr &feedback) {
            if (feedback->event_type != InteractiveMarkerFeedback::MOUSE_UP) {
                return;
            }
            
            geometry_msgs::msg::PoseStamped pose;
            pose.header = feedback->header;
            pose.header.stamp = this->get_clock()->now();
            pose.pose = feedback->pose;

            pose_pub_->publish(pose);

            RCLCPP_INFO(this->get_logger(), "Pose published");
        }
};

int main (int argc, char **argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<EEMarker>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}