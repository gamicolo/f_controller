#ifndef FUZZY_CONTROLLER_H
#define FUZZY_CONTROLLER_H

#include <Arduino.h>

#include <Fuzzy.h>

struct fuzzyInErrorRegion
{
    FuzzySet* negErr;
    FuzzySet* zeroErr;
    FuzzySet* posErr;
};

struct fuzzyInDErrorRegion
{
    FuzzySet* negDErr;
    FuzzySet* zeroDErr;
    FuzzySet* posDErr;
}; 

struct fuzzyOutRegion
{
    FuzzySet* negOut;
    FuzzySet* zeroOut;
    FuzzySet* posOut;
};

class FuzzyController{
    public:
        float procOut=0;

        FuzzyController( int inOne, int inTwo, int outOne );

        //Fuzzy controller methods 
        void begin();
        void loadInReading( const int& inReading);
        void loadInputs( const int& err, const int& dErr );
        fuzzyInErrorRegion* getInErrorRegion() const;
        fuzzyInDErrorRegion* getInDErrorRegion() const;
        fuzzyOutRegion* getOutRegion() const;
        float getOutput() const;
        void generateOutput();

        void getRulesApplied(bool []) const;


    private:
        int _inOne;
        int _inTwo;
        int _outOne;
        //static const int ARR_RULE_SIZE = 11;
        static const int ARR_RULE_SIZE = 9;

        const int ARR_FIRED_RULES = 9;

        const int ARR_IN_REGION_SIZE = 5;
        const int ARR_OUT_REGION_SIZE = 3;
        Fuzzy* _fuzzy;
        //InOut fuzzy objects
        FuzzyInput *_inFuzzyOne;
        FuzzyInput* _inFuzzyTwo;
        FuzzyOutput* _outFuzzyOne;

        FuzzyRule *_arrFuzzyRules[ARR_RULE_SIZE]={0};

        fuzzyInErrorRegion* inErrRegion;
        fuzzyInDErrorRegion* inDErrRegion;
        fuzzyOutRegion *outRegion;        


        //Fuzzy methods
        void createMembershipRegion( fuzzyInErrorRegion*, fuzzyInDErrorRegion*, fuzzyOutRegion* );
        void loadMembershipRegion( FuzzyInput*, FuzzyInput*, FuzzyOutput*, fuzzyInErrorRegion*, fuzzyInDErrorRegion*, fuzzyOutRegion* );
        void addInMemRegion( FuzzyInput*,  FuzzyInput*, fuzzyInErrorRegion*, fuzzyInDErrorRegion* );
        void addOutMemRegion( FuzzyOutput*, fuzzyOutRegion* );

        void loadVariables( Fuzzy*, FuzzyInput*, FuzzyInput*, FuzzyOutput* );

        void defineFuzzyRules( FuzzyRule *[] );
        void loadFuzzyRules( Fuzzy*, FuzzyRule *[] );
};
#endif