#pragma once
class Archive {
public:
	void zip();
	void unzip();
	void check() const;
	void info() const;
	void update();
};