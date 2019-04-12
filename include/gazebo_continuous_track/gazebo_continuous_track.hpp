#include <iostream>
#include <string>

#include <gazebo/common/common.hh>
#include <gazebo/physics/physics.hh>

#include <boost/lexical_cast.hpp>

namespace gazebo {

class ContinuousTrack : public ModelPlugin {
public:
  void Load(physics::ModelPtr parent, sdf::ElementPtr sdf) {
    //
    if (!sdf->HasElement("link")) {
      std::cerr << "No [link] element for continuous track plugin" << std::endl;
      return;
    }
    if (!sdf->HasElement("revolute_joint")) {
      std::cerr << "No [revolute_joint] element for continuous track plugin" << std::endl;
      return;
    }
    if (!sdf->HasElement("gearbox_joint")) {
      std::cerr << "No [gearbox_joint] element for continuous track plugin" << std::endl;
      return;
    }
    if (!sdf->HasElement("dpose")) {
      std::cerr << "No [dpose] element for continuous track plugin" << std::endl;
      return;
    }
    if (!sdf->HasElement("count")) {
      std::cerr << "No [count] element for continuous track plugin" << std::endl;
      return;
    }

    //
    const std::string link_name(sdf->Get< std::string >("link"));
    const physics::LinkPtr link(parent->GetLink(link_name));
    if (!link) {
      std::cerr << "no [" << link_name << "] link" << std::endl;
      return;
    }
    const sdf::ElementPtr link_sdf(link->GetSDF());

    //
    const std::string rjoint_name(sdf->Get< std::string >("revolute_joint"));
    const physics::JointPtr rjoint(parent->GetJoint(rjoint_name));
    if (!rjoint) {
      std::cerr << "no [" << rjoint_name << "] joint" << std::endl;
      return;
    }
    const sdf::ElementPtr rjoint_sdf(rjoint->GetSDF());

    //
    const std::string gjoint_name(sdf->Get< std::string >("gearbox_joint"));
    const physics::JointPtr gjoint(parent->GetJoint(gjoint_name));
    if (!gjoint) {
      std::cerr << "no [" << gjoint_name << "] joint" << std::endl;
      return;
    }
    const sdf::ElementPtr gjoint_sdf(gjoint->GetSDF());

    //
    const ignition::math::Pose3d dpose(sdf->Get< ignition::math::Pose3d >("dpose"));
    const int count(sdf->Get< int >("count"));

    //
    ignition::math::Pose3d new_link_pose(
        link_sdf->GetElement("pose")->Get< ignition::math::Pose3d >());
    for (int i = 1; i <= count; ++i) {
      //
      const std::string new_link_name(link_name + boost::lexical_cast< std::string >(i));
      const sdf::ElementPtr new_link_sdf(link_sdf->Clone());
      new_link_sdf->GetAttribute("name")->Set(new_link_name);
      new_link_pose += dpose;
      new_link_sdf->GetElement("pose")->Set(new_link_pose);
      const physics::LinkPtr new_link(parent->CreateLink(new_link_name));
      new_link->Load(new_link_sdf);
      new_link->Init();

      //
      const std::string new_rjoint_name(rjoint_name + boost::lexical_cast< std::string >(i));
      const sdf::ElementPtr new_rjoint_sdf(rjoint_sdf->Clone());
      new_rjoint_sdf->GetAttribute("name")->Set(new_rjoint_name);
      new_rjoint_sdf->GetElement("child")->Set(new_link_name);
      const physics::JointPtr new_rjoint(parent->CreateJoint(new_rjoint_sdf));
      new_rjoint->Init();

      //
      const std::string new_gjoint_name(gjoint_name + boost::lexical_cast< std::string >(i));
      const sdf::ElementPtr new_gjoint_sdf(gjoint_sdf->Clone());
      new_gjoint_sdf->GetAttribute("name")->Set(new_gjoint_name);
      new_gjoint_sdf->GetElement("child")->Set(new_link_name);
      const physics::JointPtr new_gjoint(parent->CreateJoint(new_gjoint_sdf));
      new_gjoint->Init();
    }

    std::cout << "ContinuousTrack loaded !!" << std::endl;
  }
};

} // namespace gazebo