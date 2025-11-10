import rclpy
from rclpy.node import Node
from std_msgs.msg import String

class WriterNode(Node):
    def __init__(self,name):
        super().__init__(name)
        self.get_logger().info("Hello,I'm %s" % name)
        self.pub_novel=self.create_publisher(String,"sexy_girl",10)

        self.i=0
        timer_period=5
        self.timer=self.create_timer(timer_period,self.timer_callback)

        self.account=80
        self.submonney = self.create_subscription(UInt32,"sexy_girl_money",self.recv_money_callback,10)

    def timer_callback(self):
        msg=String()
        msg.data="Chapt %d,balabala %d" % (self.i,self.i)
        self.pub_novel.publish(msg)
        self.get_logger().info("li4:I publish the %s" % msg.data)
        self.i+=1

    def recv_money_callback(self,money):
        self.account+=money.data
        self.get_logger().info('li4:I have received %d money' % self.account)

def main(args=None):
    rclpy.init(args=args)
    node=WriterNode("li4")
    rclpy.spin(node)
    rclpy.shutdown()