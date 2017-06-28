#! /bin/bash
# A simple script to build all the externals in Deployment configuration

xcodebuild -workspace dir.utils.xcworkspace -scheme max-external-all clean
xcodebuild -workspace dir.utils.xcworkspace -scheme max-external-all -configuration Deployment

