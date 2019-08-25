#include "SimpleInterface.h"

Button::Button(cv::Point position, cv::Point size, int layer) : _position(position), _size(size), _layer(layer) {

}
bool Button::IsClicked(cv::Point click) {
	return (click.x >= _position.x && click.x <= _position.x + _size.x && click.y >= _position.y && click.y <= _position.y + _size.y);
}
void Button::Draw(cv::Mat img) {
	if (img.rows < _position.y + _size.y) {
		std::cout << "Error, button out of bounds\n";
		return;
	}
}