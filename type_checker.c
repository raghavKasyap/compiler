// Group 21 
// Raghava Kasyap Kristipati - 2019A7PS0087P
// K.V.S Preetam             - 2019A7PS0030P
// Shanmukh Chandra Yama     - 2019A7PS0028P
// Uday Dheeraj Nulu         - 2019A7PS0083P
// Yadagiri Shiva Sai Sashank - 2019A7PS0068P

#include "symbol_table.h"
#include "type_checker.h"
// #include "error_handler.h"
#include <string.h>

SymbolInstance* lookupSymbolTable(SymbolTable* lookupTable, char* lexeme, SymbolTag symboltag){
    int hash = computeHash(lexeme);
    SymbolInstance* hash_list = lookupTable->symbolsHashTable[hash];

    while(hash_list != NULL){
        if(!(strcmp(hash_list->name, lexeme) && hash_list -> symbolTag == symboltag)){
            return hash_list;
        }
    }

}
int booleancheck(ASTNode* booleanexpNode,Errorlist* els){
    ASTNode* operator = booleanexpNode->children[1];
    ASTNode* left_child = NULL;
    ASTNode* right_child = NULL;
    if(operator->label == tknot){
        left_child = booleanexpNode->children[0];
        if(booleancheck(left_child))
    }
    else{
        left_child = booleanexpNode->children[0];
        right_child = booleanexpNode->children[2];
        if(left_child->label == tkid){

        }
    }
    
}
void FunctionCallCheck(ASTNode* functioncallstmtNode){
    SymbolTable* functionScopeTable =  functioncallstmtNode -> lookUpTable;
    ASTNode* outputparameters_callNode = functioncallstmtNode -> children[0];
    ASTNode* funID_NODE = functioncallstmtNode -> children[1];
    ASTNode* inputparameters_callNode = functioncallstmtNode -> children[2];

    char* functionName = funID_NODE -> astNode -> astFunIdNode -> name;
    SymbolInstance* functionDefinionRecord = lookupSymbolTable(functioncallstmtNode -> lookUpTable,functionName, functionIdentifier);
    SymbolInstance* inputparameter_funcDef = functionDefinionRecord -> symbolRecord -> functionIdentifier -> inputParameters;
    SymbolInstance* outputparameter_funcDef = functionDefinionRecord -> symbolRecord -> functionIdentifier -> outputParameters;
    
    

    
    
}

VariableIdentifier* lookupFieldList(ConstructedTypeInfoNodes* FieldList, char* fieldvariable){
    while(FieldList){
        if(fieldvariable == FieldList -> name){
            return FieldList -> variableInfo;
        }
        FieldList = FieldList -> next;
    }
    return NULL;
}

void assignmentCheck(ASTNode* assignmentNode,Errorlist* els){
    ASTNode* singleOrRecordIdNode = assignmentNode->children[0];
    ASTNode* arithmeticExpressionNode = assignmentNode -> children[2];
    ASTNode* TK_ID_Node = singleOrRecordIdNode -> children [0];
    ASTNode* constructedVariableNode = singleOrRecordIdNode -> children[1];
    

    char* lexeme = TK_ID_Node -> astNode -> astIDNode -> IdName;
    SymbolTable* lookuptable = TK_ID_Node->lookUpTable;
    SymbolTable* globaltable = lookuptable -> globalTable;
    SymbolInstance* ID_record = lookupSymbolTable(lookuptable, lexeme, variableIdentifier);
    Type id_type = ID_record -> symbolRecord -> variableIdentifier -> type;
    
    if(id_type == basic){
        if(constructedVariableNode -> numberOfChildren > 0){
            //we have to report an error citing that 
            //unapproriate access of fileds for a basic datatype
            return;
        }
        else{
            return; 
        }
    }
    else{
        ConstructedTypeInfoNodes* id_fieldList = ID_record -> symbolRecord -> variableIdentifier -> typeExpression ->typeExpressionConstructed;
        ASTNode* TK_ID_Node_children = NULL;
        char* lexeme_children = NULL;
        VariableIdentifier* field_variableInfo = NULL;
        SymbolInstance* field_Recordname_symbolInstance = NULL;
        ConstructedTypeInfoNodes* record_fields_list = NULL;
        for(int i=0;i<(constructedVariableNode->numberOfChildren)-1;i++){
            TK_ID_Node_children = (constructedVariableNode->children)[i];
            lexeme_children = TK_ID_Node_children->astNode->astIDNode->IdName;
            
           
            field_variableInfo = lookupFieldList(id_fieldList,lexeme_children);
            if(field_variableInfo == NULL){
                    // error that the field name is not present in the record
            }
            // this specifier whether type is like record a x or b and returns record or type_def accordingly
            ConstructedType child_curr_type = field_variableInfo ->typeExpression-> typeExpressionConstructed -> currType;
            char* recordName = field_variableInfo -> typeExpression->typeExpressionConstructed -> recordName;
            if(child_curr_type == UNION || child_curr_type == TAGGED_RECORD){
                break;
            }
            else if(child_curr_type == RECORD){
                field_Recordname_symbolInstance = lookupSymbolTable(globaltable,recordName, recordTypeDefinition);
                if(field_Recordname_symbolInstance == NULL){
                    // error that that field name is not present in the record
                }
                id_fieldList = field_Recordname_symbolInstance ->symbolRecord ->recordTypeDefinition ->typeExpression;
            }
            else if(child_curr_type == TYPE_DEF){
                field_Recordname_symbolInstance = lookupSymbolTable(globaltable,recordName, typedef_Definition);
                id_fieldList = field_Recordname_symbolInstance ->symbolRecord ->recordTypeDefinition ->typeExpression;
            }
        }

        field_variableInfo = lookupFieldList(id_fieldList,lexeme_children);
        if(field_variableInfo == NULL){
                // error that the field name is not present in the record
        }

        VariableIdentifier* lhs_variableId_type = field_variableInfo;
        VariableIdentifier* rhs_arithmetic_type = getArithmeticType(arithmeticExpressionNode,els) ->variableIdentifier;
        
        if(lhs_variableId_type -> type != rhs_arithmetic_type -> type){
            //have to produce type error;
        }
        else if(lhs_variableId_type == basic){

        }
        else{
            ConstructedType lhs_currtype = lhs_variableId_type->typeExpression->typeExpressionConstructed->currType;
            ConstructedType rhs_currtype = rhs_arithmetic_type->typeExpression->typeExpressionConstructed->currType;
            char* lhs_constructed_name = lhs_variableId_type->typeExpression->typeExpressionConstructed->recordName;
            char* rhs_constructed_name = rhs_arithmetic_type->typeExpression->typeExpressionConstructed->recordName;

            if( lhs_currtype == RECORD || lhs_currtype == TYPE_DEF){
                if( rhs_currtype == UNION || rhs_currtype == TAGGED_RECORD){
                    // error should be thrown
                }
                else{
                    if(rhs_currtype == RECORD && lhs_currtype == RECORD){
                        if(strcmp(lhs_constructed_name,rhs_constructed_name)){
                            // error for mismatch of record names
                        }
                    }
                    else  if(rhs_currtype == TYPE_DEF && lhs_currtype == RECORD){
                        SymbolInstance* typedef_instance = lookupSymbolTable(globaltable, rhs_constructed_name, typedef_Definition);
                        char* original_name = typedef_instance -> symbolRecord ->typedefdefinition -> originalname;
                        if(strcmp(original_name,rhs_constructed_name)){
                            // error for mismatch of record and typedef original names
                        }
                    }
                    else  if(lhs_currtype == TYPE_DEF && rhs_currtype == RECORD){
                        SymbolInstance* typedef_instance = lookupSymbolTable(globaltable, lhs_constructed_name, typedef_Definition);
                        char* original_name = typedef_instance -> symbolRecord ->typedefdefinition -> originalname;
                        if(strcmp(original_name,lhs_constructed_name)){
                            // error for mismatch of record and typedef original names
                        }
                    }
                    else  if(rhs_currtype == TYPE_DEF && lhs_currtype == TYPE_DEF){
                        SymbolInstance* typedef_instance_rhs = lookupSymbolTable(globaltable, rhs_constructed_name, typedef_Definition);
                        char* original_name_rhs = typedef_instance_rhs -> symbolRecord ->typedefdefinition -> originalname;
                        SymbolInstance* typedef_instance_lhs = lookupSymbolTable(globaltable, lhs_constructed_name, typedef_Definition);
                        char* original_name_lhs = typedef_instance_lhs -> symbolRecord ->typedefdefinition -> originalname;
                        if(strcmp(original_name_lhs,original_name_rhs)){
                            // error for mismatch of typedef original names
                        }
                    }
                }   
            }
            
            else if( lhs_currtype == UNION || lhs_currtype == TAGGED_RECORD){
                if(rhs_currtype == RECORD || rhs_currtype == TYPE_DEF){
                    // error should be thrown
                }
                else{
                    // to be handled dynamically
                }   
            }
        }

        
        
    }
}

ArithmeticTypeExpression* getArithmeticType(ASTNode* arithmeticExpressionNode, Errorlist* els){
    ASTNode* lhs = arithmeticExpressionNode -> children[0];
    ASTNode* rhs = arithmeticExpressionNode -> children[1];
    ArithmeticTypeExpression* arithmetictypeexpression;
    switch(lhs -> label){
        case tkid :{
            char* idname_lhs = lhs -> astNode -> astIDNode -> IdName;
            SymbolInstance* idSymbolRecord_lhs = lookupSymbolTable(lhs ->lookUpTable, idname_lhs,variableIdentifier);
            Type type_lhs = idSymbolRecord_lhs -> symbolRecord ->variableIdentifier ->type;
            TypeExpression* type_expression_lhs = idSymbolRecord_lhs -> symbolRecord ->variableIdentifier -> typeExpression;
            PrimitiveType basictype_lhs;
            ConstructedTypeExpression* constructedtypevariable_lhs = NULL;
            if(type_lhs == basic){
                basictype_lhs= idSymbolRecord_lhs ->symbolRecord ->variableIdentifier ->typeExpression ->typeExpressionBasic->currType;
            }
            else{
                constructedtypevariable_lhs = idSymbolRecord_lhs -> symbolRecord -> variableIdentifier -> typeExpression ->typeExpressionConstructed;
            }

            switch(rhs -> label){
                case tkid:{
                    char* idname_rhs = rhs -> astNode -> astIDNode -> IdName;
                    SymbolInstance* idSymbolRecord_rhs = lookupSymbolTable(rhs ->lookUpTable, idname_rhs,variableIdentifier);
                    Type type_rhs = idSymbolRecord_rhs -> symbolRecord ->variableIdentifier ->type;
                    TypeExpression* type_expression_rhs = idSymbolRecord_rhs -> symbolRecord ->variableIdentifier -> typeExpression;
                    PrimitiveType basictype_rhs;
                    ConstructedTypeExpression* constructedtypevariable_rhs = NULL;
                    if(type_rhs == basic){
                        basictype_rhs = idSymbolRecord_rhs ->symbolRecord ->variableIdentifier ->typeExpression ->typeExpressionBasic->currType;
                    }
                    else{
                        constructedtypevariable_rhs = idSymbolRecord_rhs -> symbolRecord -> variableIdentifier -> typeExpression ->typeExpressionConstructed;
                    }
                    
                    if(type_lhs == basic && type_rhs == recordOrUnion){
                        if(arithmeticExpressionNode -> op == multiplication && type_expression_lhs -> typeExpressionBasic -> currType == INT && type_expression_rhs -> typeExpressionConstructed -> currType == RECORD){
                            arithmetictypeexpression -> variableIdentifier = idSymbolRecord_rhs -> symbolRecord ->variableIdentifier;
                        }
                        else {
                            // report error because real lhs and record rhs (or) other operators
                        }    
                    }
                    else if(type_lhs == recordOrUnion && type_rhs == basic){

                        if(arithmeticExpressionNode -> op == multiplication && type_expression_rhs -> typeExpressionBasic -> currType == INT && type_expression_lhs -> typeExpressionConstructed -> currType == RECORD){
                            arithmetictypeexpression -> variableIdentifier = idSymbolRecord_lhs -> symbolRecord ->variableIdentifier;
                        }
                        else {
                            // report error because record lhs and real rhs (or) other operators
                        }
                    }
                    
                    else if(type_lhs == basic && type_rhs == basic){
                        // arithmetictypeexpression ->tag = VariableIdtag;
                        if(basictype_lhs == real){
                            arithmetictypeexpression -> variableIdentifier = idSymbolRecord_lhs -> symbolRecord ->variableIdentifier;
                        }
                        else{
                           arithmetictypeexpression -> variableIdentifier = idSymbolRecord_rhs -> symbolRecord ->variableIdentifier;
                        }
                    }

                    else{
                        if(type_expression_lhs -> typeExpressionConstructed -> currType == UNION || type_expression_rhs -> typeExpressionConstructed -> currType == UNION){
                            // have to return error because either of the operand is a tagged union
                        }
                        else  if(type_expression_lhs -> typeExpressionConstructed -> currType == TAGGED_RECORD || type_expression_rhs -> typeExpressionConstructed -> currType == TAGGED_RECORD){
                            // have to return error because either of the operand is a UNION
                        }
                        else if(strcmp(constructedtypevariable_lhs -> recordName, constructedtypevariable_rhs -> recordName)){
                            // have to return error
                        }
                        else if((arithmeticExpressionNode -> op == plus || arithmeticExpressionNode -> op == minus)){
                            // arithmetictypeexpression -> tag = VariableIdtag;
                            arithmetictypeexpression -> variableIdentifier = idSymbolRecord_lhs -> symbolRecord -> variableIdentifier;
                        }
                        else{
                            //have to return error
                        }
                    }
                    
                }

                case tknum:{
                    // arithmetictypeexpression -> tag = VariableIdtag;
                    if(type_lhs == basic){
                        arithmetictypeexpression -> variableIdentifier = idSymbolRecord_lhs -> symbolRecord -> variableIdentifier;
                    }
                    else if(type_expression_lhs -> typeExpressionConstructed -> currType == RECORD){
                        arithmetictypeexpression -> variableIdentifier = idSymbolRecord_lhs -> symbolRecord ->variableIdentifier;
                    }
                    
                    else{
                        //error because num is 1 operand and union or tagged union is another
                    }
                }   

                case tkrnum:{
                    // arithmetictypeexpression -> tag = VariableIdtag;
                    VariableIdentifier* variable_id = (VariableIdentifier*)malloc(sizeof(VariableIdentifier));
                    variable_id ->typeExpression = ( TypeExpression*)malloc(sizeof(TypeExpression));
                    variable_id -> typeExpression -> typeExpressionBasic = (BasicTypeExpression*)malloc(sizeof(BasicTypeExpression));
                    variable_id -> typeExpression -> typeExpressionBasic -> currType = real;
                    variable_id -> type = basic;

                    if(type_lhs == basic){
                        arithmetictypeexpression -> variableIdentifier = variable_id;
                    }
                    
                    else{
                        //error because num is 1 operand and union or tagged union is another
                    }

                }

                case arithmeticExpression:{
                    ArithmeticTypeExpression* rhs_arithmetic_type = getArithmeticType(rhs, els);
                    VariableIdentifier* rhs_arithmetic_variableIdentifier = rhs_arithmetic_type ->variableIdentifier;
                    Type type_rhs = rhs_arithmetic_variableIdentifier ->type;
                    TypeExpression* type_expression_rhs = rhs_arithmetic_variableIdentifier->typeExpression;
                    PrimitiveType basictype_rhs;
                    ConstructedTypeExpression* constructedtypevariable_rhs = NULL;
                    if(type_rhs == basic){
                        basictype_rhs = rhs_arithmetic_variableIdentifier ->typeExpression ->typeExpressionBasic->currType;
                    }
                    else{
                        constructedtypevariable_rhs =  rhs_arithmetic_variableIdentifier -> typeExpression ->typeExpressionConstructed;
                    }
                    
                    if(type_lhs == basic && type_rhs == recordOrUnion){
                        if(arithmeticExpressionNode -> op == multiplication && type_expression_lhs -> typeExpressionBasic -> currType == INT && type_expression_rhs -> typeExpressionConstructed -> currType == RECORD){
                            arithmetictypeexpression -> variableIdentifier =  rhs_arithmetic_variableIdentifier;
                        }
                        else {
                            // report error because real lhs and record rhs (or) other operators
                        } 
                       
                    }
                    else if(type_lhs == recordOrUnion && type_rhs == basic){
                        if(arithmeticExpressionNode -> op == multiplication && type_expression_rhs -> typeExpressionBasic -> currType == INT && type_expression_lhs -> typeExpressionConstructed -> currType == RECORD){
                            arithmetictypeexpression -> variableIdentifier = idSymbolRecord_lhs -> symbolRecord ->variableIdentifier;
                        }
                        else {
                            // report error because real lhs and record rhs (or) other operators
                        }    
                    }
                    
                    else if(type_lhs == basic && type_rhs == basic){
                        // BasicTypeExpression* basictypeexpression = (BasicTypeExpression*)malloc(sizeof(BasicTypeExpression));
                        if(basictype_lhs == real){
                            arithmetictypeexpression->variableIdentifier = idSymbolRecord_lhs->symbolRecord->variableIdentifier;
                        }
                        else{
                            arithmetictypeexpression ->variableIdentifier = rhs_arithmetic_variableIdentifier;
                        }
                    }

                    else{
                        // good to leave as union won't be returned in arithmetic expression, that error could have been caught at parser.
                        if(strcmp(constructedtypevariable_lhs -> recordName, constructedtypevariable_rhs -> recordName)){
                            // have to return error
                        }
                        else if (arithmeticExpressionNode -> op == plus || arithmeticExpressionNode -> op == minus){
                            arithmetictypeexpression -> variableIdentifier = rhs_arithmetic_variableIdentifier;
                            // arithmetictypeexpression -> arithmeticexpression = idSymbolRecord_lhs -> symbolRecord -> variableIdentifier;
                        }
                        else{
                            // report error
                        }
                    }
                } 

            }
        }
        case tknum:{
            switch(rhs->label){
                case tkid:{
                    char* idname_rhs = rhs -> astNode -> astIDNode -> IdName;
                    SymbolInstance* idSymbolRecord_rhs = lookupSymbolTable(rhs ->lookUpTable, idname_rhs,variableIdentifier);
                    Type type_rhs =idSymbolRecord_rhs -> symbolRecord ->variableIdentifier ->type;
                    if(type_rhs == basic){
                        //  arithmetictypeexpression -> tag = VariableIdtag;
                         arithmetictypeexpression -> variableIdentifier = idSymbolRecord_rhs -> symbolRecord -> variableIdentifier;
                    }
                    else{
                        if(arithmeticExpressionNode -> op == multiplication && idSymbolRecord_rhs -> symbolRecord -> variableIdentifier -> typeExpression -> typeExpressionConstructed -> currType == RECORD){
                            arithmetictypeexpression -> variableIdentifier =  idSymbolRecord_rhs -> symbolRecord -> variableIdentifier;
                        }
                        else{
                            // report error if scalar operation on record type is other than multi and 
                        }
                        
                    }
                }
                case tknum:{
                    // arithmetictypeexpression -> tag = NumOrRNumtag;
                    VariableIdentifier* variable_id = (VariableIdentifier*)malloc(sizeof(VariableIdentifier));
                    variable_id ->typeExpression = ( TypeExpression*)malloc(sizeof(TypeExpression));
                    variable_id -> typeExpression -> typeExpressionBasic = (BasicTypeExpression*)malloc(sizeof(BasicTypeExpression));
                    variable_id -> typeExpression -> typeExpressionBasic -> currType = INT;
                    variable_id -> type = basic;
                    
                    arithmetictypeexpression -> variableIdentifier = variable_id;

                    
                }
                case tkrnum:{
                    VariableIdentifier* variable_id = (VariableIdentifier*)malloc(sizeof(VariableIdentifier));
                    variable_id ->typeExpression = ( TypeExpression*)malloc(sizeof(TypeExpression));
                    variable_id -> typeExpression -> typeExpressionBasic = (BasicTypeExpression*)malloc(sizeof(BasicTypeExpression));
                    variable_id -> typeExpression -> typeExpressionBasic -> currType = real;
                    variable_id -> type = basic;
                    
                    arithmetictypeexpression -> variableIdentifier = variable_id;
                }
                case arithmeticExpression:{
                    ArithmeticTypeExpression* rhs_arithmetic_type = getArithmeticType(rhs, els);
                    VariableIdentifier* rhs_arithmetic_variableIdentifier = rhs_arithmetic_type ->variableIdentifier;
                    TypeExpression* type_expression_rhs = rhs_arithmetic_variableIdentifier->typeExpression;
                    if(rhs_arithmetic_type -> variableIdentifier -> type == basic){
                        arithmetictypeexpression -> variableIdentifier = rhs_arithmetic_type -> variableIdentifier;
                    }
                    else if(arithmeticExpressionNode -> op == multiplication && type_expression_rhs -> typeExpressionConstructed -> currType == RECORD){
                        arithmetictypeexpression -> variableIdentifier = rhs_arithmetic_variableIdentifier;
                    }
                    else{
                        //have to return error because of lhs tknum and rhs is record with other operators
                    }
                }
            }
        }

        case tkrnum:{
            switch(rhs->label){
                case tkid:{
                    char* idname_rhs = rhs -> astNode -> astIDNode -> IdName;
                    SymbolInstance* idSymbolRecord_rhs = lookupSymbolTable(rhs ->lookUpTable, idname_rhs,variableIdentifier);
                    Type type_rhs =idSymbolRecord_rhs -> symbolRecord ->variableIdentifier ->type;
                    if(type_rhs == basic){
                        //  arithmetictypeexpression -> tag = VariableIdtag;
                        VariableIdentifier* variable_id = (VariableIdentifier*)malloc(sizeof(VariableIdentifier));
                        variable_id ->typeExpression = ( TypeExpression*)malloc(sizeof(TypeExpression));
                        variable_id -> typeExpression -> typeExpressionBasic = (BasicTypeExpression*)malloc(sizeof(BasicTypeExpression));
                        variable_id -> typeExpression -> typeExpressionBasic -> currType = real;
                        variable_id -> type = basic; 
                        arithmetictypeexpression -> variableIdentifier = variable_id;
                    }
                    else{
                        // report error if consturcted datatype
                    }
                }

                case tknum:{
                    VariableIdentifier* variable_id = (VariableIdentifier*)malloc(sizeof(VariableIdentifier));
                    variable_id ->typeExpression = ( TypeExpression*)malloc(sizeof(TypeExpression));
                    variable_id -> typeExpression -> typeExpressionBasic = (BasicTypeExpression*)malloc(sizeof(BasicTypeExpression));
                    variable_id -> typeExpression -> typeExpressionBasic -> currType = real;
                    variable_id -> type = basic; 
                    arithmetictypeexpression -> variableIdentifier = variable_id;
   
                }
                case tkrnum:{
                    VariableIdentifier* variable_id = (VariableIdentifier*)malloc(sizeof(VariableIdentifier));
                    variable_id ->typeExpression = ( TypeExpression*)malloc(sizeof(TypeExpression));
                    variable_id -> typeExpression -> typeExpressionBasic = (BasicTypeExpression*)malloc(sizeof(BasicTypeExpression));
                    variable_id -> typeExpression -> typeExpressionBasic -> currType = real;
                    variable_id -> type = basic;
                    arithmetictypeexpression -> variableIdentifier = variable_id;
                }

                case arithmeticExpression:{
                    ArithmeticTypeExpression* rhs_arithmetic_type = getArithmeticType(rhs, els);

                    VariableIdentifier* variable_id = (VariableIdentifier*)malloc(sizeof(VariableIdentifier));
                    variable_id ->typeExpression = ( TypeExpression*)malloc(sizeof(TypeExpression));
                    variable_id -> typeExpression -> typeExpressionBasic = (BasicTypeExpression*)malloc(sizeof(BasicTypeExpression));
                    variable_id -> typeExpression -> typeExpressionBasic -> currType = real;
                    variable_id -> type = basic;
                    
                   if(rhs_arithmetic_type -> variableIdentifier -> type == basic){
                        arithmetictypeexpression -> variableIdentifier = variable_id;
                    }
                    else{
                        //have to return error
                    }
                    
                } 
            }
        }

        case arithmeticExpression:{
            ArithmeticTypeExpression* lhs_arithmetic_type = getArithmeticType(lhs, els);
            VariableIdentifier* lhs_arithmetic_variableIdentifier = lhs_arithmetic_type ->variableIdentifier;
            Type type_lhs = lhs_arithmetic_variableIdentifier ->type;
            TypeExpression* type_expression_lhs = lhs_arithmetic_variableIdentifier->typeExpression;
            PrimitiveType basictype_lhs;
            ConstructedTypeExpression* constructedtypevariable_lhs = NULL;
            if(type_lhs == basic){
                basictype_lhs = lhs_arithmetic_variableIdentifier ->typeExpression ->typeExpressionBasic->currType;
            }
            else{
                constructedtypevariable_lhs =  lhs_arithmetic_variableIdentifier -> typeExpression ->typeExpressionConstructed;
            }
            switch(rhs->label){
                case tkid:{
                    char* idname_rhs = rhs -> astNode -> astIDNode -> IdName;
                    SymbolInstance* idSymbolRecord_rhs = lookupSymbolTable(rhs ->lookUpTable, idname_rhs,variableIdentifier);
                    Type type_rhs =idSymbolRecord_rhs -> symbolRecord ->variableIdentifier ->type;
                    VariableIdentifier* rhs_arithmetic_variableIdentifier = idSymbolRecord_rhs -> symbolRecord ->variableIdentifier;
                    TypeExpression* type_expression_rhs = rhs_arithmetic_variableIdentifier->typeExpression;
                    PrimitiveType basictype_rhs;
                    ConstructedTypeExpression* constructedtypevariable_rhs = NULL;
                    if(type_rhs == basic){
                        basictype_rhs= idSymbolRecord_rhs ->symbolRecord ->variableIdentifier ->typeExpression ->typeExpressionBasic->currType;
                    }
                    else{
                        constructedtypevariable_rhs = idSymbolRecord_rhs -> symbolRecord -> variableIdentifier -> typeExpression ->typeExpressionConstructed;
                    }

                    
                    if(type_lhs == basic && type_rhs == recordOrUnion){
                        if(arithmeticExpressionNode -> op == multiplication && type_expression_lhs -> typeExpressionBasic -> currType == INT && type_expression_rhs -> typeExpressionConstructed -> currType == RECORD){
                            arithmetictypeexpression -> variableIdentifier =  rhs_arithmetic_variableIdentifier;
                        }
                        else {
                            // report error because real lhs and record rhs (or) other operators
                        } 
                       
                    }
                    else if(type_lhs == recordOrUnion && type_rhs == basic){
                        if(arithmeticExpressionNode -> op == multiplication && type_expression_rhs -> typeExpressionBasic -> currType == INT && type_expression_lhs -> typeExpressionConstructed -> currType == RECORD){
                            arithmetictypeexpression -> variableIdentifier = lhs_arithmetic_variableIdentifier;
                        }
                        else {
                            // report error because real lhs and record rhs (or) other operators
                        }    
                    }
                    
                    else if(type_lhs == basic && type_rhs == basic){
                        // BasicTypeExpression* basictypeexpression = (BasicTypeExpression*)malloc(sizeof(BasicTypeExpression));
                        if(basictype_lhs == real){
                            arithmetictypeexpression->variableIdentifier = lhs_arithmetic_variableIdentifier;
                        }
                        else{
                            arithmetictypeexpression ->variableIdentifier = rhs_arithmetic_variableIdentifier;
                        }
                    }

                    else{
                        // good to leave as union won't be returned in arithmetic expression, that error could have been caught at parser.
                        if(strcmp(constructedtypevariable_lhs -> recordName, constructedtypevariable_rhs -> recordName)){
                            // have to return error
                        }
                        else if (arithmeticExpressionNode -> op == plus || arithmeticExpressionNode -> op == minus){
                            arithmetictypeexpression -> variableIdentifier = rhs_arithmetic_variableIdentifier;
                            // arithmetictypeexpression -> arithmeticexpression = idSymbolRecord_lhs -> symbolRecord -> variableIdentifier;
                        }
                        else{
                            // report error
                        }
                    }
                }
                
                case tknum:{
                    if(type_lhs == basic){
                        arithmetictypeexpression->variableIdentifier = lhs_arithmetic_variableIdentifier;
                    }
                    else if(arithmeticExpressionNode -> op == multiplication && type_expression_lhs -> typeExpressionConstructed -> currType == RECORD){
                        arithmetictypeexpression -> variableIdentifier = lhs_arithmetic_variableIdentifier;
                    }
                    else{
                        //have to return error because of lhs tknum and rhs is record with other operators
                    }
                }
                case tkrnum:{
                    if(type_lhs == basic){
                        VariableIdentifier* variable_id = (VariableIdentifier*)malloc(sizeof(VariableIdentifier));
                        variable_id ->typeExpression = ( TypeExpression*)malloc(sizeof(TypeExpression));
                        variable_id -> typeExpression -> typeExpressionBasic = (BasicTypeExpression*)malloc(sizeof(BasicTypeExpression));
                        variable_id -> typeExpression -> typeExpressionBasic -> currType = real;
                        variable_id -> type = basic;
                        arithmetictypeexpression -> variableIdentifier = variable_id;
                    }
                    else {
                        // report error because of constructedd datatype
                    } 
                }   

                case arithmeticExpression:{
                    ArithmeticTypeExpression* rhs_arithmetic_type = getArithmeticType(rhs, els);
                    VariableIdentifier* rhs_arithmetic_variableIdentifier = rhs_arithmetic_type ->variableIdentifier;
                    Type type_rhs = rhs_arithmetic_variableIdentifier ->type;
                    TypeExpression* type_expression_rhs = rhs_arithmetic_variableIdentifier->typeExpression;
                    PrimitiveType basictype_rhs;
                    ConstructedTypeExpression* constructedtypevariable_rhs = NULL;
                    if(type_rhs == basic){
                        basictype_rhs = rhs_arithmetic_variableIdentifier ->typeExpression ->typeExpressionBasic->currType;
                    }
                    else{
                        constructedtypevariable_rhs =  rhs_arithmetic_variableIdentifier -> typeExpression ->typeExpressionConstructed;
                    }
                    
                   if(type_lhs == basic && type_rhs == recordOrUnion){
                       if(arithmeticExpressionNode -> op == multiplication && type_expression_lhs -> typeExpressionBasic -> currType == INT &&  type_expression_rhs -> typeExpressionConstructed -> currType == RECORD){
                            arithmetictypeexpression -> variableIdentifier =  rhs_arithmetic_variableIdentifier;
                        }
                        else {
                            // report error because real lhs and record rhs (or) other operators
                        } 
                    }
                    else if(type_lhs == recordOrUnion && type_lhs == basic){
                        if(arithmeticExpressionNode -> op == multiplication && type_expression_lhs -> typeExpressionBasic -> currType == INT && type_expression_lhs -> typeExpressionConstructed -> currType == RECORD){
                            arithmetictypeexpression -> variableIdentifier =  rhs_arithmetic_variableIdentifier;
                        }
                        else {
                            // report error because real lhs and record rhs (or) other operators
                        } 
                    }
                    
                    else if(type_lhs == basic && type_rhs == basic){
                        // BasicTypeExpression* basictypeexpression = (BasicTypeExpression*)malloc(sizeof(BasicTypeExpression));
                        if(basictype_rhs == real){
                            arithmetictypeexpression->variableIdentifier = lhs_arithmetic_variableIdentifier;
                        }
                        else{
                            arithmetictypeexpression ->variableIdentifier = lhs_arithmetic_variableIdentifier;
                        }       
                    }

                    else{
                        // good to leave as union and tagged union wont be returned from arithmetic expression
                        if(strcmp(constructedtypevariable_lhs -> recordName, constructedtypevariable_rhs -> recordName)){
                            // have to return error
                        }
                        else{
                            arithmetictypeexpression -> variableIdentifier = lhs_arithmetic_variableIdentifier ;
                            // arithmetictypeexpression -> arithmeticexpression = idSymbolRecord_lhs -> symbolRecord -> variableIdentifier;
                        }
                    }
                }
            }
        }

    }
}

