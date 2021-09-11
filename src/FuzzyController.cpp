#include "FuzzyController.h"
#include <Arduino.h>

FuzzyController::FuzzyController( int inOne, int inTwo, int outOne ) : _inOne(inOne),_inTwo(inTwo), _outOne(outOne)
{
    outRegion = new fuzzyOutRegion();
    inErrRegion = new fuzzyInErrorRegion();
    inDErrRegion = new fuzzyInDErrorRegion();
    //Fuzzy object
    _fuzzy = new Fuzzy();
    _inFuzzyOne = new FuzzyInput(_inOne);
    _inFuzzyTwo = new FuzzyInput(_inTwo);
    _outFuzzyOne = new FuzzyOutput(_outOne);    
}


void FuzzyController::begin()
{
    FuzzyController::createMembershipRegion( inErrRegion, inDErrRegion, outRegion);
    FuzzyController::loadMembershipRegion( _inFuzzyOne, _inFuzzyTwo, _outFuzzyOne, inErrRegion, inDErrRegion, outRegion);
    FuzzyController::loadVariables(_fuzzy, _inFuzzyOne, _inFuzzyTwo,_outFuzzyOne );
    FuzzyController::defineFuzzyRules( _arrFuzzyRules );
    FuzzyController::loadFuzzyRules( _fuzzy, _arrFuzzyRules );
}

void FuzzyController::createMembershipRegion( fuzzyInErrorRegion* inErrR, fuzzyInDErrorRegion* inDErrR, fuzzyOutRegion* outR )
{
    //In membership region
    inErrR->negErr = new FuzzySet(-30,-30,-8,0);//quisiera tener un rango de -10 a -2
    inErrR->zeroErr = new FuzzySet(-1,0,0,1);//quisiera tener un rango de -3 a 3
    inErrR->posErr = new FuzzySet(0,8,60,60);//quisiera tener un rango de 2 a 10

    inDErrR->negDErr = new FuzzySet(-1,-1,-1,0);//quisiera tener un rango de -5 a 0
    inDErrR->zeroDErr = new FuzzySet(-0.5,0,0,0.5);//rango de -1 a 1
    inDErrR->posDErr = new FuzzySet(0,1,1,1);//rango de 0 a 5
    
    //Out membership region
    outR->negOut = new FuzzySet(-1, -0.5, -0.5, 0);
    outR->zeroOut = new FuzzySet(-0.5, 0, 0, 0.5);
    outR->posOut = new FuzzySet(0, 0.5, 0.5, 1);


    /*
    //In membership region
    inErrR->negErr = new FuzzySet(-30,-30,-10,-2);//quisiera tener un rango de -10 a -2
    inErrR->zeroErr = new FuzzySet(-3,0,0,3);//quisiera tener un rango de -3 a 3
    inErrR->posErr = new FuzzySet(2,10,60,60);//quisiera tener un rango de 2 a 10

    inDErrR->negDErr = new FuzzySet(-5,-2.5,-2.5,0);//quisiera tener un rango de -5 a 0
    inDErrR->zeroDErr = new FuzzySet(-1,0,0,1);//rango de -1 a 1
    inDErrR->posDErr = new FuzzySet(0,2.5,2.5,5);//rango de 0 a 5
    
    //Out membership region
    outR->negOut = new FuzzySet(-1, -0.5, -0.5, 0);
    outR->zeroOut = new FuzzySet(-0.5, 0, 0, 0.5);
    outR->posOut = new FuzzySet(0, 0.5, 0.5, 1);

    //In membership region
    inErrR->negErr = new FuzzySet(-60,-60,-10,0);
    inErrR->zeroErr = new FuzzySet(-1,0,0,1);
    inErrR->posErr = new FuzzySet(0,10,60,60);

    inDErrR->negDErr = new FuzzySet(-2,-1,-1,0);
    inDErrR->zeroDErr = new FuzzySet(0,0,0,0);
    inDErrR->posDErr = new FuzzySet(0,1,1,2);

    //Out membership region
    outR->negOut = new FuzzySet(-2, -1, -1, 0);
    outR->zeroOut = new FuzzySet(-1, 0, 0, 1);
    outR->posOut = new FuzzySet(0, 1, 1, 2);
    */

    /*
    outR->negOut = new FuzzySet(-128, -64, -64, 0);
    outR->zeroOut = new FuzzySet(-64, 0, 0, 64);
    outR->posOut = new FuzzySet(0, 64, 64, 128);
    */
}

void FuzzyController::loadMembershipRegion( FuzzyInput* fInOne, FuzzyInput* fInTwo, FuzzyOutput* fOutOne, fuzzyInErrorRegion* inErrR, fuzzyInDErrorRegion* inDErrR, fuzzyOutRegion* outR )
{
    FuzzyController::addInMemRegion( fInOne, fInTwo, inErrR, inDErrR);
    FuzzyController::addOutMemRegion( fOutOne, outR );
}

void FuzzyController::addInMemRegion( FuzzyInput *fInOne, FuzzyInput* fInTwo, fuzzyInErrorRegion* inErrR, fuzzyInDErrorRegion* inDErrR )
{
    fInOne->addFuzzySet(inErrR->negErr);
    fInOne->addFuzzySet(inErrR->zeroErr);
    fInOne->addFuzzySet(inErrR->posErr);

    fInTwo->addFuzzySet(inDErrR->negDErr);
    fInTwo->addFuzzySet(inDErrR->zeroDErr);
    fInTwo->addFuzzySet(inDErrR->posDErr);
}

void FuzzyController::addOutMemRegion( FuzzyOutput* fOutOne, fuzzyOutRegion* outR )
{
    fOutOne->addFuzzySet(outR->negOut);
    fOutOne->addFuzzySet(outR->zeroOut);
    fOutOne->addFuzzySet(outR->posOut);
}

void FuzzyController::loadVariables( Fuzzy* fuzzy, FuzzyInput* fInOne, FuzzyInput* fInTwo, FuzzyOutput* fOutOne )
{
    fuzzy->addFuzzyInput(fInOne);
    fuzzy->addFuzzyInput(fInTwo);
    fuzzy->addFuzzyOutput(fOutOne);
}

//fuzzyRules is an array of pointers
void FuzzyController::defineFuzzyRules( FuzzyRule *fuzzyRules[] )
//void FuzzyController::defineFuzzyRules( FuzzyRule *fuzzyRules[], fuzzyInErrorRegion* inErrR, fuzzyInDErrorRegion* inDErrR, fuzzyOutRegion* outR )
{

    FuzzyRuleAntecedent* ifErrPositiveAndDErrZero = new FuzzyRuleAntecedent();
    FuzzyRuleAntecedent* ifErrPositiveAndDErrNegative = new FuzzyRuleAntecedent();
    FuzzyRuleAntecedent* ifErrPositiveAndDErrPositive = new FuzzyRuleAntecedent();
    
    FuzzyRuleAntecedent* ifErrNegativeAndDErrZero = new FuzzyRuleAntecedent();
    FuzzyRuleAntecedent* ifErrNegativeAndDErrNegative = new FuzzyRuleAntecedent();
    FuzzyRuleAntecedent* ifErrNegativeAndDErrPositive = new FuzzyRuleAntecedent();

    FuzzyRuleAntecedent* ifErrZeroAndDErrZero = new FuzzyRuleAntecedent();
    FuzzyRuleAntecedent* ifErrZeroAndDErrNegative = new FuzzyRuleAntecedent();
    FuzzyRuleAntecedent* ifErrZeroAndDErrPositive = new FuzzyRuleAntecedent();

    FuzzyRuleConsequent* thenOutNegative = new FuzzyRuleConsequent();
    FuzzyRuleConsequent* thenOutPositive = new FuzzyRuleConsequent();
    FuzzyRuleConsequent* thenOutZero = new FuzzyRuleConsequent();

    ifErrPositiveAndDErrZero->joinWithAND(inErrRegion->posErr,inDErrRegion->zeroDErr);
    ifErrPositiveAndDErrNegative->joinWithAND(inErrRegion->posErr,inDErrRegion->negDErr);
    ifErrPositiveAndDErrPositive->joinWithAND(inErrRegion->posErr,inDErrRegion->posDErr);

    ifErrNegativeAndDErrZero->joinWithAND(inErrRegion->negErr,inDErrRegion->zeroDErr);
    ifErrNegativeAndDErrNegative->joinWithAND(inErrRegion->negErr,inDErrRegion->negDErr);
    ifErrNegativeAndDErrPositive->joinWithAND(inErrRegion->negErr,inDErrRegion->posDErr);

    ifErrZeroAndDErrZero->joinWithAND(inErrRegion->zeroErr,inDErrRegion->zeroDErr);
    ifErrZeroAndDErrNegative->joinWithAND(inErrRegion->zeroErr,inDErrRegion->negDErr);
    
    thenOutPositive->addOutput(outRegion->posOut);
    thenOutNegative->addOutput(outRegion->negOut);
    thenOutZero->addOutput(outRegion->zeroOut);

    fuzzyRules[0] = new FuzzyRule(0,ifErrNegativeAndDErrNegative,thenOutNegative);
    fuzzyRules[1] = new FuzzyRule(1,ifErrNegativeAndDErrZero,thenOutNegative);
    fuzzyRules[2] = new FuzzyRule(2,ifErrNegativeAndDErrPositive,thenOutNegative);//1
    //fuzzyRules[2] = new FuzzyRule(2,ifErrNegativeAndDErrPositive,thenOutPositive);//1 esto no seria correcto
    //fuzzyRules[2] = new FuzzyRule(2,ifErrNegativeAndDErrPositive,thenOutZero);//1 puede ser esta opcion tambien

    fuzzyRules[3] = new FuzzyRule(3,ifErrPositiveAndDErrNegative,thenOutPositive);//2 en que caso se puede dar?
    //fuzzyRules[3] = new FuzzyRule(3,ifErrPositiveAndDErrNegative,thenOutZero);//2
    fuzzyRules[4] = new FuzzyRule(4,ifErrPositiveAndDErrZero,thenOutPositive);
    fuzzyRules[5] = new FuzzyRule(5,ifErrPositiveAndDErrPositive,thenOutPositive);
    
    fuzzyRules[6] = new FuzzyRule(6,ifErrZeroAndDErrNegative,thenOutNegative);
    fuzzyRules[7] = new FuzzyRule(7,ifErrZeroAndDErrZero,thenOutZero);
    fuzzyRules[8] = new FuzzyRule(8,ifErrZeroAndDErrPositive,thenOutZero);//puede ser tambien salida negativa|

    /*

    fuzzyRules[0] = new FuzzyRule(0,ifErrPositiveAndDErrZero,thenOutPositive);
    fuzzyRules[1] = new FuzzyRule(1,ifErrZeroAndDErrNegative,thenOutNegative);
    fuzzyRules[2] = new FuzzyRule(2,ifErrNegativeAndDErrZero,thenOutNegative);
    fuzzyRules[3] = new FuzzyRule(3,ifErrNegativeAndDErrPositive,thenOutPositive);//1
    fuzzyRules[4] = new FuzzyRule(4,ifErrZeroAndDErrZero,thenOutZero);
    fuzzyRules[5] = new FuzzyRule(5,ifErrPositiveAndDErrNegative,thenOutPositive);//2
    fuzzyRules[6] = new FuzzyRule(6,ifErrNegativeAndDErrNegative,thenOutNegative);
    fuzzyRules[7] = new FuzzyRule(7,ifErrNegativeAndDErrPositive,thenOutNegative);//1
    fuzzyRules[8] = new FuzzyRule(8,ifErrPositiveAndDErrPositive,thenOutPositive);
    fuzzyRules[9] = new FuzzyRule(9,ifErrPositiveAndDErrNegative,thenOutZero);//2
    fuzzyRules[10] = new FuzzyRule(10,ifErrNegativeAndDErrPositive,thenOutZero);//1

    */
}

void FuzzyController::loadFuzzyRules( Fuzzy* fuzzy, FuzzyRule *arrFuzzyRules[] )
{
    for(int i = 0; i < ARR_RULE_SIZE; i++)
        fuzzy-> addFuzzyRule(arrFuzzyRules[i]);
}

void FuzzyController::loadInReading( const int& inReading )
{
    _fuzzy->setInput(_inOne, inReading);
    _fuzzy->fuzzify();
}

void FuzzyController::loadInputs( const int& err, const int& derr )
{
    _fuzzy->setInput(_inOne, err);
    _fuzzy->setInput(_inTwo, derr);
    _fuzzy->fuzzify();
}

fuzzyInErrorRegion* FuzzyController::getInErrorRegion() const
{
    return inErrRegion;
}

fuzzyInDErrorRegion* FuzzyController::getInDErrorRegion() const
{
    return inDErrRegion;
}

fuzzyOutRegion* FuzzyController::getOutRegion() const
{
    return outRegion;
}

void FuzzyController::generateOutput()
{
    procOut = _fuzzy->defuzzify(_outOne);
}

float FuzzyController::getOutput() const
{
    return procOut;
}

void FuzzyController::getRulesApplied(bool rules[]) const
{
    for(int i = 0; i < ARR_RULE_SIZE; i++)
        rules[i] = _fuzzy->isFiredRule(i);
}