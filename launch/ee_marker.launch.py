from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import ExecuteProcess

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='marker_node',
            executable='ee_marker_node',
            name='ee_marker_node',
            output='screen',
        ),
        ExecuteProcess(
            cmd=['rviz2'],
            output='screen',
        )
    ])