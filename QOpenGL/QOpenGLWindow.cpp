#include "QOpenGLWindow.h"

void QOpenGLWindow::initializeGL()
{
	this->initializeOpenGLParameters();
	this->initializeOpenGLFunctions();
	glPolygon=new QOpenGLPolygon(quads, colors, coords);
	glTexture=newQOpenGLTexture("texture.jpg");
	glProgram=newQOpenGLShaderProgram("shader.v", "shader.f");
	glProgram->setUniformValue("decayVector", QVector3D(0.5, 0.1, 0));
	glProgram->setUniformValue("lightPosition", QVector3D(-5, 5, 5));
	glProgram->setUniformValue("lightColor", QVector3D(2, 2, 2));
	glProgram->setUniformValue("eyePosition", eye);
	glProgram->setUniformValue("shininess", 5.5f);
	glProgram->enableAttributeArray("texcoord");
	glProgram->enableAttributeArray("position");
	glProgram->enableAttributeArray("normal");
	glProgram->enableAttributeArray("color");
	glProgram->setUniformValue("texture", 0);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
}
void QOpenGLWindow::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glModelMatrix.setToIdentity();
	glViewMatrix.setToIdentity();
	glModelMatrix.scale(5);
	glModelMatrix.rotate(rotateAngle, QVector3D(0, 1, 0));
	glViewMatrix.lookAt(eye, center, up);
	glProgram->setUniformValue("eyePosition", eye);
	glProgram->setUniformValue("normalMatrix", glModelMatrix.normalMatrix());
	glProgram->setUniformValue("modelMatrix", glModelMatrix);
	glProgram->setUniformValue("viewMatrix", glViewMatrix);
	glPolygon->indexBuffer->bind();  glTexture->bind();
	glBindAttributeBuffer("texcoord", glPolygon->texcoordBuffer, 2);
	glBindAttributeBuffer("position", glPolygon->positionBuffer, 3);
	glBindAttributeBuffer("normal", glPolygon->normalBuffer, 3);
	glBindAttributeBuffer("color", glPolygon->colorBuffer, 3);
	glDrawElements(GL_TRIANGLES, glPolygon->indexLength, GL_UNSIGNED_SHORT, 0);
}
void QOpenGLWindow::resizeGL(int width, int height)
{
	glProjectionMatrix.setToIdentity();
	glProjectionMatrix.perspective(fov, (width+0.0)/height, zNear, zFar);
	glProgram->setUniformValue("projectionMatrix", glProjectionMatrix);
}
void QOpenGLWindow::teardownGL()
{
	delete glPolygon;
	delete glTexture;
	delete glProgram;
}
QOpenGLWindow::~QOpenGLWindow()
{
	makeCurrent();
	teardownGL();
}
void QOpenGLWindow::initializeOpenGLParameters()
{
	this->rotateAngle=0;
	this->angularSpeed=0*0.6;
	this->translateSpeed=0.5;
	this->rotateSpeed=0.4;
	this->viewDistance=6;
	this->right.setX(1);
	this->up.setY(1); 
	this->forward.setZ(-1);
	this->eye.setZ(viewDistance);
	this->basicTimer.start(15, this);
}
QOpenGLShaderProgram* QOpenGLWindow::newQOpenGLShaderProgram(QString vShader, QString fShader)
{
	QOpenGLShaderProgram* program=new QOpenGLShaderProgram();
	program->addShaderFromSourceFile(QOpenGLShader::Vertex, vShader);
	program->addShaderFromSourceFile(QOpenGLShader::Fragment, fShader);
	program->link(); program->bind(); return program;
}
QOpenGLTexture* QOpenGLWindow::newQOpenGLTexture(QString fileName)
{
	QOpenGLTexture* texture = new QOpenGLTexture(QImage(fileName));
	texture->setMinificationFilter(QOpenGLTexture::Linear);
	texture->setMagnificationFilter(QOpenGLTexture::Linear);
	texture->setWrapMode(QOpenGLTexture::Repeat);
	return texture;
}
void QOpenGLWindow::glBindAttributeBuffer(const char* name, QOpenGLBuffer* glBuffer, GLuint tupleSize)
{
	glBuffer->bind();
	glProgram->setAttributeBuffer(name, GL_FLOAT, 0, tupleSize, sizeof(GLfloat)*tupleSize);
}
void QOpenGLWindow::updateViewDirection()
{
	qreal rotUp=-rotateSpeed*mouseMove.x();
	qreal rotRight=-rotateSpeed*mouseMove.y();
	this->forward=rotate(forward, up, rotUp);
	this->right=rotate(right, up, rotUp);
	this->forward=rotate(forward, right, rotRight);
	this->up=rotate(up, right, rotRight);
	this->eye=center-viewDistance*forward;
}
void QOpenGLWindow::updateViewDistance()
{
	this->viewDistance-=translateSpeed*mouseDelta/120;
	this->viewDistance=clamp(viewDistance, 4, 20);
	this->eye=center-viewDistance*forward;
}
QVector3D QOpenGLWindow::rotate(QVector3D vector, QVector3D axis, qreal angle)
{
	rotateMatrix.setToIdentity();
	rotateMatrix.rotate(angle, axis.x(), axis.y(), axis.z());
	return rotateMatrix.map(vector);
}
void QOpenGLWindow::mouseMoveEvent(QMouseEvent* event)
{
	if(isMousePressed)
	{
		this->mouseMove=event->pos()-mousePos;
		this->mousePos=event->pos();
		this->updateViewDirection();
	}
}
void QOpenGLWindow::mousePressEvent(QMouseEvent* event)
{
	if(event->button()==Qt::LeftButton) 
	{
		this->isMousePressed=true;
		this->mousePos=event->pos();
	}
}
void QOpenGLWindow::mouseReleaseEvent(QMouseEvent* event)
{
	if(isMousePressed)
	{
        		this->isMousePressed=false;
	}
}
void QOpenGLWindow::wheelEvent(QWheelEvent* event)
{
	this->mouseDelta=event->delta();
	this->updateViewDistance();
	this->mouseDelta=0;
}
void QOpenGLWindow::timerEvent(QTimerEvent* event)
{
	this->rotateAngle+=angularSpeed;
	this->update();
}
qreal QOpenGLWindow::clamp(qreal value, qreal min, qreal max)
{
	return value<min?min:value>max?max:value;
}
