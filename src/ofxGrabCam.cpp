//
//  ofxGrabCam.cpp
//  ofxSketchupCamera
//
//  Created by Elliot Woods on 10/11/2011.
//  Copyright (c) 2011 Kimchi and Chips. All rights reserved.
//

#include "ofxGrabCam.h"

//--------------------------
ofxGrabCam::ofxGrabCam() : initialised(true), mouseDown(false), pickCursorFlag(false), drawCursor(false), drawCursorSize(0.1) {
	addListeners();
}

//--------------------------
ofxGrabCam::~ofxGrabCam() {
	//removing events actually seems to upset something
	//removeListeners();
}

//--------------------------
void ofxGrabCam::init() {
	ofCamera::setNearClip(0.5);
	
	addListeners();
	reset();
}

//--------------------------
void ofxGrabCam::begin(ofRectangle viewport) {
	glEnable(GL_DEPTH_TEST);	
	ofCamera::begin(viewport);
}

//--------------------------
void ofxGrabCam::end() {
	if (pickCursorFlag || !mouseDown) {
		findCursor();
		pickCursorFlag = false;
	}
	
	// this has to happen after all drawing + findCursor()
	// but before camera.end()
	if (drawCursor) {
		ofPushStyle();
		ofSetColor(0, 0, 0);
		ofSphere(mouseW.x, mouseW.y, mouseW.z, drawCursorSize);
		ofPopStyle();
	}
	
	ofCamera::end();
	glDisable(GL_DEPTH_TEST);
}

//--------------------------
void ofxGrabCam::reset() {
	ofCamera::resetTransform();
}

//--------------------------
void ofxGrabCam::setCursorDraw(bool enabled, float size) {
	this->drawCursor = enabled;
	this->drawCursorSize = size;
}

//--------------------------
void ofxGrabCam::toggleCursorDraw() {
	this->drawCursor ^= true;
}

//--------------------------
void ofxGrabCam::addListeners() {
	ofAddListener(ofEvents.update, this, &ofxGrabCam::update);
    ofAddListener(ofEvents.mouseMoved, this, &ofxGrabCam::mouseMoved);
    ofAddListener(ofEvents.mousePressed, this, &ofxGrabCam::mousePressed);
    ofAddListener(ofEvents.mouseReleased, this, &ofxGrabCam::mouseReleased);
    ofAddListener(ofEvents.mouseDragged, this, &ofxGrabCam::mouseDragged);
    ofAddListener(ofEvents.keyPressed, this, &ofxGrabCam::keyPressed);

	initialised = true;
}

//--------------------------
void ofxGrabCam::removeListeners() {
	if (!initialised)
		return;
	
	ofRemoveListener(ofEvents.update, this, &ofxGrabCam::update);
    ofRemoveListener(ofEvents.mouseMoved, this, &ofxGrabCam::mouseMoved);
    ofRemoveListener(ofEvents.mousePressed, this, &ofxGrabCam::mousePressed);
    ofRemoveListener(ofEvents.mouseReleased, this, &ofxGrabCam::mouseReleased);
    ofRemoveListener(ofEvents.mouseDragged, this, &ofxGrabCam::mouseDragged);
    ofRemoveListener(ofEvents.keyPressed, this, &ofxGrabCam::keyPressed);
	
	initialised = false;
}

//--------------------------
void ofxGrabCam::update(ofEventArgs &args) {
	
}

//--------------------------
void ofxGrabCam::mouseMoved(ofMouseEventArgs &args) {
	mouseP.x = args.x;
	mouseP.y = args.y;
}

//--------------------------
void ofxGrabCam::mousePressed(ofMouseEventArgs &args) {
	mouseP.x = args.x;
	mouseP.y = args.y;
	
	if (!mouseDown)
		pickCursorFlag = true;
	mouseDown = true;
}

//--------------------------
void ofxGrabCam::mouseReleased(ofMouseEventArgs &args) {
	mouseDown = false;
}

//--------------------------
void ofxGrabCam::mouseDragged(ofMouseEventArgs &args) {
	float dx = args.x - mouseP.x;
	float dy = args.y - mouseP.y;
	
	mouseP.x = args.x;
	mouseP.y = args.y;
	
	
	if (mouseP.z == 1.0f)
		return;
	
	ofVec3f p = ofCamera::getPosition();
	ofVec3f uy = 2 * ofCamera::getUpDir();
	ofVec3f ux = 2 * ofCamera::getSideDir();
	
	if (args.button==0) {		
		rotation.makeRotate(dx/ofGetWidth() * 90, -uy, dy/ofGetHeight() * 90, -ux, 0, ofVec3f(0,0,1));
		ofCamera::setPosition((p - mouseW) * rotation + mouseW);
		ofCamera::rotate(rotation);
	} else {
		ofCamera::move(2 * (mouseW - p) * dy / ofGetHeight());
	}
}

//--------------------------
void ofxGrabCam::keyPressed(ofKeyEventArgs &args) {
	if (args.key == 'r')
		reset();
}

//--------------------------
void ofxGrabCam::findCursor() {
	//read z value from depth buffer at mouse coords
	glReadPixels(mouseP.x, ofGetViewportHeight()-1-mouseP.y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &mouseP.z);
	glGetDoublev(GL_PROJECTION_MATRIX, matP);
	glGetDoublev(GL_MODELVIEW_MATRIX, matM);
	glGetIntegerv(GL_VIEWPORT, viewport);
	
	GLdouble c[3];
	
	gluUnProject(mouseP.x, ofGetViewportHeight()-1-mouseP.y, mouseP.z, matM, matP, viewport, c, c+1, c+2);
	
	mouseW.x = c[0];
	mouseW.y = c[1];
	mouseW.z = c[2];
}