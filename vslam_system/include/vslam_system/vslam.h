#ifndef VSLAM_SYSTEM_VSLAM_H
#define VSLAM_SYSTEM_VSLAM_H

#include <vslam_system/vo.h>
#include <vslam_system/place_recognizer.h>
#include <posest/pe3d.h>
#include <sba/sba.h>
#include <frame_common/frame.h>

namespace vslam {

/// \brief VSLAM class that performs visual odometry, loop closure through 
/// place recognition, and large-scale optimization using SBA.
class VslamSystem
{
  public: /// @todo Leave these protected
    frame_common::FrameProc frame_processor_; ///< Frame processor for new frames.
    /// \brief Image frames in system.
    std::vector<frame_common::Frame, Eigen::aligned_allocator<frame_common::Frame> > frames_;
    sba::SysSBA sba_;	 ///< Large-scale SBA system.
    vslam::voSt vo_;   ///< Visual odometry processor.
    vslam::PlaceRecognizer place_recognizer_; ///< Place recognizer
    pe::PoseEstimator3d pose_estimator_;      ///< For place recognition matches

  public:
    /// \brief Constructor for VslamSystem.
    /// \param vocab_tree_file Path to the vocabulary tree file.
    /// \param vocab_weights_file Path to the vocabulary tree weights file.
    VslamSystem(const std::string& vocab_tree_file, const std::string& vocab_weights_file);

    /// \brief Add a frame to the system.
    /// \param camera_parameters Camera parameters for the cameras.
    /// \param left Left image.
    /// \param right Right image.
    /// \param nfrac Fractional disparity. If above 0, then right is an int16_t 
    ///              disparity image.
    bool addFrame(const frame_common::CamParams& camera_parameters,
                  const cv::Mat& left, const cv::Mat& right, int nfrac = 0);

    /// \brief Perform a refinement on the large-scale SBA system.
    void refine();

    int prInliers;  ///< Number of inliers needed for PR match.
    int numPRs;			///< Number of place recognitions that succeeded.

    // parameters settings
    void setPlaceInliers(int n) { prInliers = n; }; ///< Place recognition inliers.
    void setKeyInliers(int n) { vo_.mininls = n; }; ///< Set keyframe inliers.
    void setKeyDist(double n) { vo_.mindist = n; }; ///< Set minimum keyframe distance in meters.
    void setKeyAngle(double n) { vo_.minang = n; }; ///< Set minimum keyframe angle in radians.
    void setNDisp(int n) { frame_processor_.ndisp = n; }; ///< Set the number of disparities in stereo processing.
    void setPRRansacIt(int n) { pose_estimator_.numRansac = n; }; ///< Set the number of RANSAC iterations for place recognition pose estimate.
    void setPRPolish(bool n) { pose_estimator_.polish = n; }; ///< Set whether to polish the place recognition pose estimate using SBA.
    void setVORansacIt(int n) { vo_.pose_estimator_->numRansac = n; }; ///< Set the number of RANSAC iterations for visual odometry pose estimate.
    void setVOPolish(bool n) { vo_.pose_estimator_->polish = n; }; ///< Set whether to polish pose estimate for visual odometry using SBA.
};

} // namespace vslam

#endif