#pragma once
class MObject
{
private:
	static unsigned int CurrentID;
	unsigned int instanceID;
	bool avaliable;
protected:
	virtual void Dispose() {}
public:
	unsigned int GetInstanceID() const { return instanceID; }
	bool Avaliable() const { return avaliable; }
	MObject();
	void Release() {
		if (avaliable) {
			avaliable = false;
			Dispose();
		}
	}
	virtual ~MObject()
	{
		Release();
	}
};

