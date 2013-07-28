using namespace Sim;
using namespace Sim::ShapeGen;

TargetStorage::TargetStorage()
{
}

TargetStorage & TargetStorage::instance()
{
	static TargetStorage targetStorage;
	return targetStorage;
}